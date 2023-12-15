#pragma once
#include "swapchain.h"

namespace KNR
{
	class VulkanSwapchain : public Swapchain
	{
		struct VulkanSwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};
	public:
		VulkanSwapchain(void* windowPtr, void* instance, int screenWidth, int screenHeight);
		VulkanSwapchain(void* windowPtr, int screenWidth, int screenHeight);
		~VulkanSwapchain();
		VkSwapchainKHR GetSwapchain(){return m_swapChain; }
	private:
		void Initialize();
		void QuerySwapChainSupport();
		VkSurfaceFormatKHR  ChooseSurfaceFormat();
		VkPresentModeKHR ChoosePresentMode();
		VkExtent2D ChooseSwapchainExtent();

		VkSwapchainKHR m_swapChain;

		VkSurfaceKHR m_surface;
		VulkanSwapChainSupportDetails m_swapChainSupportDetails;
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;

		//Wtf is this man? 
		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;
		std::vector<VkFramebuffer> m_swapChainFramebuffers;
	};
}

