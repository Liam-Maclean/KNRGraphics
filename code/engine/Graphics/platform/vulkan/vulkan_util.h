#pragma once
#include <vulkan/vulkan.h>
#include "logging/log.h"
#include "graphics/platform/vulkan/vulkan_device.h"
#include <functional>

namespace Qualia
{
	namespace Vulkan
	{

		//FrameBuffer attachment Wrapper
		struct FrameBufferAttachment
		{
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
			VkFormat format;
		};


		class Utility
		{
		public:
			static Utility& GetInstance()
			{
				static Utility instance;
				return instance;
			}

			QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice device);
			void _CreateCommandPool();
			
			void _ImmediateSubmit(std::function<void (VkCommandBuffer cmd)>&& function);
			VkCommandBuffer _BeginSingleTimeCommands();
			void _EndSingleTimeCommands(VkCommandBuffer commandBuffer);
			void _CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
			void _CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
			void _CreateShaderBuffer(VkDevice device, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory, VkBufferUsageFlagBits bufferStage, void* data);
			void _CreateAttachment(VkFormat format, VkImageUsageFlagBits usageFlags, FrameBufferAttachment* frameBufferAttachment, int width, int height);
			void _TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
			static void VulkanErrorCheck(VkResult result);

			VkCommandPool GetCommandPool(){ return m_CommandPool; };

		private:

			Utility();
			~Utility();
			VkCommandPool m_CommandPool;
		
		};
	}
}