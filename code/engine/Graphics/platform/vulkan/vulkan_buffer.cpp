#include "vulkan_buffer.h"
#include "vulkan_graphics_context.h"

namespace KNR
{
	Buffer* Buffer::Create(const BufferDescriptor& desc)
	{
		return new VulkanBuffer(desc);
	}

	VulkanBuffer::VulkanBuffer(const BufferDescriptor& desc)
	{
		
		switch (desc.bufferAccessType)
		{
		case BufferAccessType::DEFAULT:
			break;
		case BufferAccessType::UPLOAD:
			break;
		case BufferAccessType::READBACK:
			break;
		case BufferAccessType::CUSTOM:
			break;
		default:
			break;
		}

		if (desc.debugName)
		{
			
		}

		//Create the views for the buffer
		if (desc.bufferType == BufferUsageType::VERTEX)
		{
			
		}
		else if (desc.bufferType == BufferUsageType::INDEX)
		{
			
		}
		else if (desc.bufferType == BufferUsageType::UNIFORM)
		{
			
		}
		else if (desc.bufferType == BufferUsageType::STORAGE)
		{
			
		}

		//If we have initial data, we need to stage
		if (desc.initialData)
		{

		}
	}

	VulkanBuffer::~VulkanBuffer()
	{
		
	}

	void VulkanBuffer::Bind(DirectXCommandBuffer* commandList) const
	{

	}

	void VulkanBuffer::Unbind(DirectXCommandBuffer* commandList) const
	{

	}

	void* VulkanBuffer::Map()
	{
		void* data;

		return data;
	}

	void VulkanBuffer::UnMap()
	{

	}

	uint32_t VulkanBuffer::AppendData(uint32_t size, void* data)
	{
		uint32_t finalSize;

		return finalSize;
	}
}