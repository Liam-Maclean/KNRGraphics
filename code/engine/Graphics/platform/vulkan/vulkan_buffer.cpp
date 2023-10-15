#include "vulkan_buffer.h"
#include "vulkan_graphics_context.h"
#include "vk_mem_alloc.h"

namespace KNR
{
	Buffer* Buffer::Create(const BufferDescriptor& desc)
	{
		return new VulkanBuffer(desc);
	}

	VulkanBuffer::VulkanBuffer(const BufferDescriptor& desc)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = desc.size;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		
		switch (desc.bufferAccessType)
		{
		case BufferAccessType::DEFAULT:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
			break;
		case BufferAccessType::UPLOAD:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
			break;
		case BufferAccessType::READBACK:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
			break;
		case BufferAccessType::CUSTOM:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
			break;
		default:
			break;
		}

		//Create the views for the buffer
		if (desc.bufferType == BufferUsageType::VERTEX)
		{
			bufferInfo.usage = bufferInfo.usage | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		else if (desc.bufferType == BufferUsageType::INDEX)
		{
			bufferInfo.usage = bufferInfo.usage | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		else if (desc.bufferType == BufferUsageType::UNIFORM)
		{
			bufferInfo.usage = bufferInfo.usage | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		else if (desc.bufferType == BufferUsageType::STORAGE)
		{
			bufferInfo.usage = bufferInfo.usage | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		//If we have initial data, we need to stage
		if (desc.initialData)
		{
			//We can't fill this yet, I don't remember how staging buffers work and which flags are needed
		}

		//With one function call:  
		//	VkBuffer is created
		//	VkDeviceMemory is allocated if needed
		//	An unused region of the memory block is bound to this buffer
		//
		//vmaAllocation can be used to access the VkDeviceMemory handle and it's offset
		vmaCreateBuffer(VulkanContext.GetVMAAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);

		//TODO: Vulkan likes to be difficult and has way too much boilerplate for simple stuff like name tagging
		if (desc.debugName)
		{

		}
	}

	VulkanBuffer::~VulkanBuffer()
	{
		
	}

	void VulkanBuffer::Bind(CommandBuffer* commandList) const
	{

	}

	void VulkanBuffer::Unbind(CommandBuffer* commandList) const
	{

	}

	void* VulkanBuffer::Map()
	{
		void* data;

		vmaMapMemory(VulkanContext.GetVMAAllocator(), m_allocation, &data);

		return data;
	}

	void VulkanBuffer::UnMap()
	{
		vmaUnmapMemory(VulkanContext.GetVMAAllocator(), m_allocation);
	}

	uint32_t VulkanBuffer::AppendData(uint32_t size, void* data)
	{
		uint32_t finalSize;

		return finalSize;
	}
}