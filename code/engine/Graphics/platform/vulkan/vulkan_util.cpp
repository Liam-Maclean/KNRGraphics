#include "vulkan_util.h"
#include "vulkan_device.h"


namespace Qualia
{
	namespace Vulkan
	{
		void Utility::VulkanErrorCheck(VkResult result)
		{
			if (result < 0)
			{
				switch (result)
				{
				case VK_ERROR_OUT_OF_HOST_MEMORY:
					Q_CORE_ERROR("VK_ERROR_OUT_OF_HOST_MEMORY");
					break;
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:
					Q_CORE_ERROR("VK_ERROR_OUT_OF_DEVICE_MEMORY");
					break;
				case VK_ERROR_INITIALIZATION_FAILED:
					Q_CORE_ERROR("VK_ERROR_INITIALIZATION_FAILED");
					break;
				case VK_ERROR_DEVICE_LOST:
					Q_CORE_ERROR("VK_ERROR_DEVICE_LOST");
					break;
				case VK_ERROR_MEMORY_MAP_FAILED:
					Q_CORE_ERROR("VK_ERROR_MEMORY_MAP_FAILED");
					break;
				case VK_ERROR_LAYER_NOT_PRESENT:
					Q_CORE_ERROR("VK_ERROR_LAYER_NOT_PRESENT");
					break;
				case VK_ERROR_EXTENSION_NOT_PRESENT:
					Q_CORE_ERROR("VK_ERROR_EXTENSION_NOT_PRESENT");
					break;
				case VK_ERROR_FEATURE_NOT_PRESENT:
					Q_CORE_ERROR("VK_ERROR_FEATURE_NOT_PRESENT");
					break;
				case VK_ERROR_INCOMPATIBLE_DRIVER:
					Q_CORE_ERROR("VK_ERROR_INCOMPATIBLE_DRIVER");
					break;
				case VK_ERROR_TOO_MANY_OBJECTS:
					Q_CORE_ERROR("VK_ERROR_TOO_MANY_OBJECTS");
					break;
				case VK_ERROR_FORMAT_NOT_SUPPORTED:
					Q_CORE_ERROR("VK_ERROR_FORMAT_NOT_SUPPORTED");
					break;
				case VK_ERROR_FRAGMENTED_POOL:
					Q_CORE_ERROR("VK_ERROR_FRAGMENTED_POOL");
					break;
				case VK_ERROR_OUT_OF_POOL_MEMORY:
					Q_CORE_ERROR("VK_ERROR_OUT_OF_POOL_MEMORY");
					break;
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:
					Q_CORE_ERROR("VK_ERROR_INVALID_EXTERNAL_HANDLE");
					break;
				case VK_ERROR_SURFACE_LOST_KHR:
					Q_CORE_ERROR("VK_ERROR_SURFACE_LOST_KHR");
					break;
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
					Q_CORE_ERROR("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");
					break;
				case VK_SUBOPTIMAL_KHR:
					Q_CORE_ERROR("VK_SUBOPTIMAL_KHR");
					break;
				case VK_ERROR_OUT_OF_DATE_KHR:
					Q_CORE_ERROR("VK_ERROR_OUT_OF_DATE_KHR");
					break;
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
					Q_CORE_ERROR("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
					break;
				case VK_ERROR_VALIDATION_FAILED_EXT:
					Q_CORE_ERROR("VK_ERROR_VALIDATION_FAILED_EXT");
					break;
				default:
					break;
				}
				assert(0 && "Vulkan runtime error.");
			}
		}

		Utility::Utility()
		{
			_CreateCommandPool();
		}

		Utility::~Utility()
		{

		}

		//Method to find and filter the queue families of the device we have chosen
		QueueFamilyIndices  Utility::_FindQueueFamilies(VkPhysicalDevice device) {
			QueueFamilyIndices indices;

			//query device for family queue support properties
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			//loop through queue families to find specific queue types and flags
			for (const auto& queueFamily : queueFamilies) {
				if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					indices.graphicsFamily = i;
				}

				////query the physical device for surface support
				//VkBool32 presentSupport = VK_NULL_HANDLE;
				//vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

				//if suitable
				if (queueFamily.queueCount > 0)
				{
					indices.presentFamily = i;
				}

				if (indices.isComplete()) {
					//break out of the loop and return indices
					break;
				}

				i++;
			}

			return indices;
		}

		//Method for creating command pools for queues
		void Utility::_CreateCommandPool()
		{
			QueueFamilyIndices queueFamilyIndices = _FindQueueFamilies(VulkanDevice::GetInstance().GetVulkanPhysicalDevice());

			VkCommandPoolCreateInfo command_pool_create_info{};
			command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			command_pool_create_info.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
			command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			Qualia::Vulkan::Utility::VulkanErrorCheck(vkCreateCommandPool(VulkanDevice::GetInstance().GetVulkanDevice(), &command_pool_create_info, nullptr, &m_CommandPool));
		}

		void Utility::_ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
		{
			//allocate the default command buffer that we will use for the instant commands
			VkCommandBufferAllocateInfo cmdAllocInfo;// = vkinit::command_buffer_allocate_info(_uploadContext._commandPool, 1);
			cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			cmdAllocInfo.commandBufferCount = 1;
			cmdAllocInfo.commandPool = m_CommandPool;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(VulkanDevice::GetInstance().GetVulkanDevice(), &cmdAllocInfo, &commandBuffer);

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &begin_info);

			//execute the function
			function(commandBuffer);


			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submit_info = {};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(VulkanDevice::GetInstance().GetVulkanGraphicsQueue(), 1, &submit_info, VK_NULL_HANDLE);
			vkQueueWaitIdle(VulkanDevice::GetInstance().GetVulkanGraphicsQueue());

			//clear the command pool. This will free the command buffer too
			vkResetCommandPool(VulkanDevice::GetInstance().GetVulkanDevice(), m_CommandPool, 0);
		}

		//Method to create a single time use command buffer
		VkCommandBuffer Utility::_BeginSingleTimeCommands()
		{
			VkCommandBufferAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			alloc_info.commandPool = m_CommandPool;
			alloc_info.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(VulkanDevice::GetInstance().GetVulkanDevice(), &alloc_info, &commandBuffer);

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &begin_info);

			return commandBuffer;

		}

		//Method to end the single time command buffer
		void Utility::_EndSingleTimeCommands(VkCommandBuffer commandBuffer)
		{
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submit_info = {};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(VulkanDevice::GetInstance().GetVulkanGraphicsQueue(), 1, &submit_info, VK_NULL_HANDLE);
			vkQueueWaitIdle(VulkanDevice::GetInstance().GetVulkanGraphicsQueue());

			vkFreeCommandBuffers(VulkanDevice::GetInstance().GetVulkanDevice(), m_CommandPool, 1, &commandBuffer);
		}


		//Method to transition image layout
		void Utility::_TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
		{
			VkCommandBuffer commandBuffer = _BeginSingleTimeCommands();
			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = image;

			if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				//if (_HasStencilComponent(format)) {
				//	barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				//}
			}
			else {
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}

			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = 0;

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			{
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}
			else
			{
				throw std::invalid_argument("Unsupported Layout Transition!");
			}

			vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			_EndSingleTimeCommands(commandBuffer);
		}


		//Method for creating a VkBuffer
		void Utility::_CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
		{
			VkBufferCreateInfo buffer_create_info = {};
			buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_create_info.size = size;
			buffer_create_info.usage = usage;
			buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			Qualia::Vulkan::Utility::VulkanErrorCheck(vkCreateBuffer(VulkanDevice::GetInstance().GetVulkanDevice(), &buffer_create_info, nullptr, &buffer));

			VkMemoryRequirements mem_requirements;
			vkGetBufferMemoryRequirements(VulkanDevice::GetInstance().GetVulkanDevice(), buffer, &mem_requirements);

			VkMemoryAllocateInfo memory_allocate_info = {};
			memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memory_allocate_info.allocationSize = mem_requirements.size;
			memory_allocate_info.memoryTypeIndex = VulkanDevice::GetInstance()._GetMemoryType(mem_requirements.memoryTypeBits, properties);

			Qualia::Vulkan::Utility::VulkanErrorCheck(vkAllocateMemory(VulkanDevice::GetInstance().GetVulkanDevice(), &memory_allocate_info, nullptr, &bufferMemory));

			vkBindBufferMemory(VulkanDevice::GetInstance().GetVulkanDevice(), buffer, bufferMemory, 0);
		}

		//Method to copy the buffer to VRAM
		void Utility::_CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
		{
			VkCommandBuffer commandBuffer = _BeginSingleTimeCommands();

			VkBufferCopy copyRegion = {};
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			_EndSingleTimeCommands(commandBuffer);
		}

		//*Performs the staging buffer process to move to GPU memory
		void Utility::_CreateShaderBuffer(VkDevice device, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory, VkBufferUsageFlagBits bufferStage, void* data)
		{
			VkDeviceSize bufferSize = size;

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			_CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* tempData;
			vkMapMemory(VulkanDevice::GetInstance().GetVulkanDevice(), stagingBufferMemory, 0, bufferSize, 0, &tempData);
			memcpy(tempData, data, (size_t)bufferSize);
			vkUnmapMemory(VulkanDevice::GetInstance().GetVulkanDevice(), stagingBufferMemory);

			_CreateBuffer(bufferSize, bufferStage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *buffer, *memory);
			_CopyBuffer(stagingBuffer, *buffer, bufferSize);

			vkDestroyBuffer(VulkanDevice::GetInstance().GetVulkanDevice(), stagingBuffer, nullptr);
			vkFreeMemory(VulkanDevice::GetInstance().GetVulkanDevice(), stagingBufferMemory, nullptr);
		}

		void Utility::_CreateAttachment(VkFormat format, VkImageUsageFlagBits usageFlags, FrameBufferAttachment* frameBufferAttachment, int width, int height)
		{
			//Initialise local variables
			VkImageAspectFlags aspectMask = 0;
			VkImageLayout imageLayout;

			//Set the attachment format
			frameBufferAttachment->format = format;

			//if the attachment we're using is not the depth test
			if (usageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			{
				//set the aspect mask to image color bit
				aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			//if the attachment we're using is the depth test
			if (usageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				//set the depth test mask and image stencil flags and change image layout to depth stencil optimal
				aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}

			//make sure the aspect mask is greater than 0 (Depth test or image color attachment flags have been met)
			assert(aspectMask > 0);

			//Set up create info for the image for the framebuffer using format, and framebuffer we have
			VkImageCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.format = format;
			imageCreateInfo.extent.width = width;
			imageCreateInfo.extent.height = height;
			imageCreateInfo.extent.depth = 1;
			imageCreateInfo.mipLevels = 1;
			imageCreateInfo.arrayLayers = 1;
			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.usage = usageFlags | VK_IMAGE_USAGE_SAMPLED_BIT;
			//Initialise mem alloc and mem requirement variables
			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			VkMemoryRequirements memReqs;

			//Create the image for the framebuffer and check the memory requirements to bind and allocate memory for the image
			VulkanErrorCheck(vkCreateImage(VulkanDevice::GetInstance().GetVulkanDevice(), &imageCreateInfo, nullptr, &frameBufferAttachment->image));
			vkGetImageMemoryRequirements(VulkanDevice::GetInstance().GetVulkanDevice(), frameBufferAttachment->image, &memReqs);
			memAlloc.allocationSize = memReqs.size;
			memAlloc.memoryTypeIndex = VulkanDevice::GetInstance()._GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VulkanErrorCheck(vkAllocateMemory(VulkanDevice::GetInstance().GetVulkanDevice(), &memAlloc, nullptr, &frameBufferAttachment->mem));
			VulkanErrorCheck(vkBindImageMemory(VulkanDevice::GetInstance().GetVulkanDevice(), frameBufferAttachment->image, frameBufferAttachment->mem, 0));

			//Set up image view create info to create the view for the image buffer
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = format;
			imageViewCreateInfo.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			imageViewCreateInfo.subresourceRange = {};
			imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;
			imageViewCreateInfo.image = frameBufferAttachment->image;

			//Create the image view for the frameBufferAttachment
			VulkanErrorCheck(vkCreateImageView(VulkanDevice::GetInstance().GetVulkanDevice(), &imageViewCreateInfo, nullptr, &frameBufferAttachment->view));
		}
	}
}