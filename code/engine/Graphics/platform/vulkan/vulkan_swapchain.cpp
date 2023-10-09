#include "vulkan_swapchain.h"
#include "vulkan_graphics_context.h"
#include "vulkan_util.h"
namespace KNR
{
	VulkanSwapchain::VulkanSwapchain(void* windowPtr, int screenWidth, int screenHeight)
	{
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
		swap_chain_create_info.surface = _surface;
		swap_chain_create_info.minImageCount = imageCount;
		swap_chain_create_info.imageFormat = surfaceFormat.format;
		swap_chain_create_info.imageColorSpace = surfaceFormat.colorSpace;
		swap_chain_create_info.imageExtent = extent;

		//specifies the amount of layers each image can consist of (always 1 unless steroscopic 3D)
		swap_chain_create_info.imageArrayLayers = 1;

		//what kind of operations the image in the swap chain are for (come back to when post processing)
		//if post processing would use VK_IMAGE_USAGE_TRANSFER_DST_BIT and use memory operation to transfer image
		swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = _FindQueueFamilies(_renderer->GetVulkanPhysicalDevice());
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

		Util::ErrorCheck(vkCreateSwapchainKHR(_renderer->GetVulkanDevice(), &swap_chain_create_info, nullptr, &_swapChain));

		vkGetSwapchainImagesKHR(_renderer->GetVulkanDevice(), _swapChain, &imageCount, nullptr);
		_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(_renderer->GetVulkanDevice(), _swapChain, &imageCount, _swapChainImages.data());

		_swapChainImageFormat = surfaceFormat.format;
		_swapChainExtent = extent;
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		vkDestroySwapchainKHR(CVulkanContext::Get()->GetDevice(), m_swapChain, nullptr);
	}
}
