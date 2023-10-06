#pragma once
#include "vulkan/vulkan.h"
#include "vulkan_graphics_pipeline.h"
#include <vector>


struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


class VulkanSwapchain
{
public:
	VulkanSwapchain();
	~VulkanSwapchain();

	static VulkanSwapchain GetInstance()
	{
		static VulkanSwapchain sSwapchain;
		return sSwapchain;
	}

	VkSwapchainKHR GetSwapchain() { return m_Swapchain; };
	VkFormat GetSwapchainFormat() { return m_SwapchainImageFormat; }
	VkExtent2D GetSwapchainExtent() { return m_SwapchainExtent; }
	std::vector<VkFramebuffer> GetSwapchainFramebuffers() { return m_SwapchainFramebuffers; }
	std::vector<VkImage> GetSwapchainImages() { return m_SwapchainImages; }
	std::vector<VkImageView> GetSwapchainImageViews() { return m_SwapchainImageViews; }
	VkPipeline GetSwapchainGraphicsPipeline() { return m_SwapchainGraphicsPipeline->GetGraphicsPipeline();};
	VkPipelineLayout GetSwapchainGraphicsPipelineLayout() { return m_SwapchainGraphicsPipeline->GetPipelineLayout(); };
	VkRenderPass GetSwapchainRenderPass() { return m_SwapchainGraphicsPipeline->GetRenderPass(); };

private:
	void Init();
	void CreateSwapchain();
	void CreateSwapchainFramebuffers();
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void CreateColorResources();
	void CreateDepthResources();
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void CreateImageViews();

	SwapChainSupportDetails _QuerySwapchainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR _ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR _ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D _ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	//We may as well keep this here if it renders everything belonging to the base swapchain
	VulkanGraphicsPipeline* m_SwapchainGraphicsPipeline;
	
	//Swapchain formats and depth imaging
	VkImage m_ColorImage;
	VkDeviceMemory m_ColorImageMemory;
	VkImageView m_ColorImageView;

	VkImage m_DepthImage;
	VkDeviceMemory m_DepthImageMemory;
	VkImageView m_DepthImageView;

	//==Swapchain==
	VkSwapchainKHR m_Swapchain;
	std::vector<VkImage> m_SwapchainImages;
	std::vector<VkImageView> m_SwapchainImageViews;
	VkFormat m_SwapchainImageFormat;
	VkExtent2D m_SwapchainExtent;
	std::vector <VkFramebuffer> m_SwapchainFramebuffers;
	VkSurfaceKHR m_Surface;
};