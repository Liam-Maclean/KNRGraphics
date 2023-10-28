#define VMA_IMPLEMENTATION
#include "vulkan_swapchain.h"
#include "vulkan_graphics_context.h"
#include "vulkan_util.h"
#include "vulkan/vulkan_win32.h"
#include <window.h>

namespace KNR
{
	VulkanSwapchain::VulkanSwapchain(void* windowPtr, void* instance, int screenWidth, int screenHeight)
	{
		VkInstance vulkanInstance = VulkanContext.Get()->GetInstance();

		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = (HWND)(windowPtr);
		createInfo.hinstance = (HINSTANCE)(instance);

		if (vkCreateWin32SurfaceKHR(vulkanInstance, &createInfo, nullptr, &m_surface) != VK_SUCCESS) {

		}
	}

	VulkanSwapchain::VulkanSwapchain(void* windowPtr, int screenWidth, int screenHeight)
	{
		
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		vkDestroySwapchainKHR(VulkanContext.Get()->GetDevice(), m_swapChain, nullptr);
	}

	void VulkanSwapchain::Initialize()
	{
		VkDevice device = VulkanContext.Get()->GetDevice();

		VulkanSwapChainSupportDetails swapChainSupport;

		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat();
		VkPresentModeKHR presentMode = ChoosePresentMode();
		VkExtent2D extent = ChooseSwapchainExtent();

		//if the value of the maxImageCount is 0 then there is no limit to image count
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			//if the max image count has a limit, set it to the maxImageCount
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR swap_chain_create_info = {};
		swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swap_chain_create_info.surface = m_surface;
		swap_chain_create_info.minImageCount = imageCount;
		swap_chain_create_info.imageFormat = surfaceFormat.format;
		swap_chain_create_info.imageColorSpace = surfaceFormat.colorSpace;
		swap_chain_create_info.imageExtent = extent;

		//specifies the amount of layers each image can consist of (always 1 unless steroscopic 3D)
		swap_chain_create_info.imageArrayLayers = 1;

		//what kind of operations the image in the swap chain are for (come back to when post processing)
		//if post processing would use VK_IMAGE_USAGE_TRANSFER_DST_BIT and use memory operation to transfer image
		swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t presentFamilyIdx = VulkanContext.Get()->GetPresentQueueIndex();
		uint32_t graphicsFamilyIdx = VulkanContext.Get()->GetGraphicsQueueIndex();
		uint32_t queueFamilyIndices[] = { (uint32_t)presentFamilyIdx, (uint32_t)graphicsFamilyIdx };

		//*VK_SHARING_MODE_EXCLUSIVE = image is ownd by one family at a time and ownership 
		//							   must be transfered
		//*VK_SHARING_MODE_CONCURRENT = Images can be used across mltiple queue families
		//							   without ownership transfer
		if (graphicsFamilyIdx != presentFamilyIdx)
		{
			swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swap_chain_create_info.queueFamilyIndexCount = 2;
			swap_chain_create_info.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		//any transforms (90 degree flips or anything for post processing)
		swap_chain_create_info.preTransform = swapChainSupport.capabilities.currentTransform;
		swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swap_chain_create_info.presentMode = presentMode;

		//if window is ontop and pixels are obscured then don't read those pixels
		swap_chain_create_info.clipped = VK_TRUE;

		//if swapchain becomes invalid or unoptimised while running you can
		//recreate the swapchain or reference an old one 
		swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

		Util::ErrorCheck(vkCreateSwapchainKHR(device, &swap_chain_create_info, nullptr, &m_swapChain));

		vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, nullptr);
		m_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, m_swapChainImages.data());

		m_swapChainImageFormat = surfaceFormat.format;
		m_swapChainExtent = extent;
	}

	void VulkanSwapchain::QuerySwapChainSupport()
	{
		VkPhysicalDevice device;

		//query the surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &m_swapChainSupportDetails.capabilities);

		//Query the surface formats from the device
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

		//if there is more than one surface format
		if (formatCount != 0)
		{
			//resize and add the formats to the structure
			m_swapChainSupportDetails.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, m_swapChainSupportDetails.formats.data());
		}

		//Query the presentation modes on the device
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			m_swapChainSupportDetails.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, m_swapChainSupportDetails.presentModes.data());
		}
	}
	VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat()
	{
		//if the surface has no prefered format
		if (m_swapChainSupportDetails.formats.size() == 1 && m_swapChainSupportDetails.formats[0].format == VK_FORMAT_UNDEFINED)
		{
			//Use nonlinear standard
			return { VK_FORMAT_B8G8R8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		//if it has no prefered format loop through available formats
		for (const auto& availableFormat : m_swapChainSupportDetails.formats)
		{
			//if UNORM and NONLINEAR_KHR is available
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				//return that format
				return availableFormat;
			}
		}

		//if all else fails return the first format that exists
		return m_swapChainSupportDetails.formats[0];
	}
	VkPresentModeKHR VulkanSwapchain::ChoosePresentMode()
	{
		//*VK_PRESENT_MODE_IMMEDIATE_KHR = Images submitted and transfered to screen straight away
			//*VK_PRESENT_MODE_FIFO_KHR = Swapchain queue, images at front of queue are taken when display is refreshed
			//*VK_PRESENT_MODE_FIFO_RELAXED_KHR = Like Previous but if application late and queue empty transfer right away
			//*VK_PRESENT_MODE_MAILBOX_KHR = like FIFO (normal) but used for triple buffering methods

		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		//loop through available present modes
		for (const auto& availablePresentMode : m_swapChainSupportDetails.presentModes)
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

	VkExtent2D VulkanSwapchain::ChooseSwapchainExtent()
	{
		if (m_swapChainSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return m_swapChainSupportDetails.capabilities.currentExtent;
		}
		else
		{
			int width = 1280;
			int height = 720;
			//TODO
			//glfwGetFramebufferSize(, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::max(m_swapChainSupportDetails.capabilities.minImageExtent.width, std::min(m_swapChainSupportDetails.capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(m_swapChainSupportDetails.capabilities.minImageExtent.height, std::min(m_swapChainSupportDetails.capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}
}
