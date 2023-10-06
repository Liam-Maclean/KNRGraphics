#include "vulkan_swapchain.h"
#include "vulkan_util.h"
#include "vulkan_device.h"
#include "application/application.h"


VulkanSwapchain::VulkanSwapchain()
{
	//m_SwapchainGraphicsPipeline = new VulkanGraphicsPipeline("assets\shaders\default_color.vert.spv", "assets\shaders\default_color.frag.spv");
	Init();
}

VulkanSwapchain::~VulkanSwapchain()
{
	delete m_SwapchainGraphicsPipeline;
	m_SwapchainGraphicsPipeline = 0;
}

void VulkanSwapchain::Init()
{
	CreateSwapchain();
	CreateImageViews();
	CreateColorResources();
	CreateDepthResources();
	CreateSwapchainFramebuffers();
}

void VulkanSwapchain::CreateSwapchain()
{
	//Create the surface from the GLFW application window
	Qualia::Application& app = Qualia::Application::Get();
	GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
	Qualia::Vulkan::Utility::VulkanErrorCheck(glfwCreateWindowSurface(VulkanDevice::GetInstance().GetVulkanInstance(), window, nullptr, &m_Surface));


	SwapChainSupportDetails swapChainSupport = _QuerySwapchainSupport(VulkanDevice::GetInstance().GetVulkanPhysicalDevice());
	VkSurfaceFormatKHR surfaceFormat = _ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = _ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = _ChooseSwapExtent(swapChainSupport.capabilities);


	//if the value of the maxImageCount is 0 then there is no limit to image count
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		//if the max image count has a limit, set it to the maxImageCount
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swap_chain_create_info = {};
	swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swap_chain_create_info.surface = m_Surface;
	swap_chain_create_info.minImageCount = imageCount;
	swap_chain_create_info.imageFormat = surfaceFormat.format;
	swap_chain_create_info.imageColorSpace = surfaceFormat.colorSpace;
	swap_chain_create_info.imageExtent = extent;

	//specifies the amount of layers each image can consist of (always 1 unless steroscopic 3D)
	swap_chain_create_info.imageArrayLayers = 1;

	//what kind of operations the image in the swap chain are for (come back to when post processing)
	//if post processing would use VK_IMAGE_USAGE_TRANSFER_DST_BIT and use memory operation to transfer image
	swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = VulkanDevice::GetInstance()._FindQueueFamilies(VulkanDevice::GetInstance().GetVulkanPhysicalDevice());
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swap_chain_create_info.queueFamilyIndexCount = 2;
		swap_chain_create_info.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	//*VK_SHARING_MODE_EXCLUSIVE = image is ownd by one family at a time and ownership 
	//							   must be transfered
	//*VK_SHARING_MODE_CONCURRENT = Images can be used across mltiple queue families
	//							   without ownership transfer

	//any transforms (90 degree flips or anything for post processing)
	swap_chain_create_info.preTransform = swapChainSupport.capabilities.currentTransform;
	swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swap_chain_create_info.presentMode = presentMode;

	//if window is ontop and pixels are obscured then don't read those pixels
	swap_chain_create_info.clipped = VK_TRUE;

	//if swapchain becomes invalid or unoptimised while running you can
	//recreate the swapchain or reference an old one 
	swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

	Qualia::Vulkan::Utility::VulkanErrorCheck(vkCreateSwapchainKHR(VulkanDevice::GetInstance().GetVulkanDevice(), &swap_chain_create_info, nullptr, &m_Swapchain));

	vkGetSwapchainImagesKHR(VulkanDevice::GetInstance().GetVulkanDevice(), m_Swapchain, &imageCount, nullptr);
	m_SwapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(VulkanDevice::GetInstance().GetVulkanDevice(), m_Swapchain, &imageCount, m_SwapchainImages.data());

	m_SwapchainImageFormat = surfaceFormat.format;
	m_SwapchainExtent = extent;
}

void VulkanSwapchain::CreateSwapchainFramebuffers()
{
	m_SwapchainFramebuffers.resize(m_SwapchainImageViews.size());

	for (size_t i = 0; i < m_SwapchainImageViews.size(); i++)
	{
		std::array<VkImageView, 3> attachments = {
			m_ColorImageView,
			m_DepthImageView,
			m_SwapchainImageViews[i],
		};

		VkFramebufferCreateInfo framebuffer_create_info = {};
		framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_create_info.pNext = 0;
		framebuffer_create_info.renderPass = m_SwapchainGraphicsPipeline->GetRenderPass();
		framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebuffer_create_info.pAttachments = attachments.data();
		framebuffer_create_info.width = m_SwapchainExtent.width;
		framebuffer_create_info.height = m_SwapchainExtent.height;
		framebuffer_create_info.layers = 1;

		Qualia::Vulkan::Utility::VulkanErrorCheck(vkCreateFramebuffer(VulkanDevice::GetInstance().GetVulkanDevice(), &framebuffer_create_info, nullptr, &m_SwapchainFramebuffers[i]));
	}
}

void VulkanSwapchain::CreateImageViews()
{
	//resize the swap chain image views to the amount of swapchain images
	m_SwapchainImageViews.resize(m_SwapchainImages.size());

	//loop through swapchain images
	for (size_t i = 0; i < m_SwapchainImages.size(); i++)
	{
		m_SwapchainImageViews[i] = CreateImageView(m_SwapchainImages[i], m_SwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void VulkanSwapchain::CreateColorResources()
{
	VkFormat colorFormat = m_SwapchainImageFormat;

	CreateImage(m_SwapchainExtent.width, m_SwapchainExtent.height, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ColorImage, m_ColorImageMemory);
	m_ColorImageView = CreateImageView(m_ColorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	Qualia::Vulkan::Utility::GetInstance()._TransitionImageLayout(m_ColorImage, colorFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void VulkanSwapchain::CreateDepthResources()
{
	VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

	CreateImage(m_SwapchainExtent.width, m_SwapchainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
	m_DepthImageView = CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	Qualia::Vulkan::Utility::GetInstance()._TransitionImageLayout(m_DepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void VulkanSwapchain::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo image_create_info = {};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.extent.width = width;
	image_create_info.extent.height = height;
	image_create_info.extent.depth = 1;
	image_create_info.mipLevels = 1;
	image_create_info.arrayLayers = 1;
	image_create_info.format = format;
	image_create_info.tiling = tiling;
	image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_create_info.usage = usage;
	image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;

	Qualia::Vulkan::Utility::VulkanErrorCheck(vkCreateImage(VulkanDevice::GetInstance().GetVulkanDevice(), &image_create_info, nullptr, &image));

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(VulkanDevice::GetInstance().GetVulkanDevice(), image, &memRequirements);

	VkMemoryAllocateInfo memory_alloc_info = {};
	memory_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_alloc_info.allocationSize = memRequirements.size;
	memory_alloc_info.memoryTypeIndex = VulkanDevice::GetInstance()._GetMemoryType(memRequirements.memoryTypeBits, properties);

	Qualia::Vulkan::Utility::VulkanErrorCheck(vkAllocateMemory(VulkanDevice::GetInstance().GetVulkanDevice(), &memory_alloc_info, nullptr, &imageMemory));

	vkBindImageMemory(VulkanDevice::GetInstance().GetVulkanDevice(), image, imageMemory, 0);
}

VkImageView VulkanSwapchain::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;
	view_info.components = {
		VK_COMPONENT_SWIZZLE_R,
		VK_COMPONENT_SWIZZLE_G,
		VK_COMPONENT_SWIZZLE_B,
		VK_COMPONENT_SWIZZLE_A
	};
	view_info.subresourceRange.aspectMask = aspectFlags;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	VkImageView imageView;
	Qualia::Vulkan::Utility::VulkanErrorCheck(vkCreateImageView(VulkanDevice::GetInstance().GetVulkanDevice(), &view_info, nullptr, &imageView));

	return imageView;
}

SwapChainSupportDetails VulkanSwapchain::_QuerySwapchainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	//query the surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);


	//Query the surface formats from the device
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

	//if there is more than one surface format
	if (formatCount != 0)
	{
		//resize and add the formats to the structure
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
	}


	//Query the presentation modes on the device
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR VulkanSwapchain::_ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//if the surface has no prefered format
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		//Use nonlinear standard
		return { VK_FORMAT_B8G8R8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	//if it has no prefered format loop through available formats
	for (const auto& availableFormat : availableFormats)
	{
		//if UNORM and NONLINEAR_KHR is available
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			//return that format
			return availableFormat;
		}
	}

	//if all else fails return the first format that exists
	return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::_ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	//*VK_PRESENT_MODE_IMMEDIATE_KHR = Images submitted and transfered to screen straight away
	//*VK_PRESENT_MODE_FIFO_KHR = Swapchain queue, images at front of queue are taken when display is refreshed
	//*VK_PRESENT_MODE_FIFO_RELAXED_KHR = Like Previous but if application late and queue empty transfer right away
	//*VK_PRESENT_MODE_MAILBOX_KHR = like FIFO (normal) but used for triple buffering methods

	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	//loop through available present modes
	for (const auto& availablePresentMode : availablePresentModes)
	{
		//if mailbox is available
		if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR)
		{
			//return that available present mode
			return availablePresentMode;
		}
		//otherwise if available presentMode is immediate, change to that
		//if mailbox isn't available
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	//return default FIFO if fail to find prefered type
	return bestMode;
}

VkExtent2D VulkanSwapchain::_ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		Qualia::Application& app = Qualia::Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}