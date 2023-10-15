#pragma once
#include <cstdint>
#include "buffer.h"
#include "vulkan_graphics_context.h"
namespace KNR
{
	class VulkanBuffer
		: public Buffer
	{
	public:
		VulkanBuffer(const BufferDescriptor& desc);
		~VulkanBuffer();

		virtual void Bind(DirectXCommandBuffer* commandList) const override;
		virtual void Unbind(DirectXCommandBuffer* commandList) const override;

		virtual void* Map() override;
		virtual void UnMap() override;

		virtual size_t GetCapacitySize() override { return m_allocatedSize; }
		virtual size_t GetUsedSize() override { return m_usedSize; }

		virtual uint32_t AppendData(uint32_t size, void* data) override;
		VkBuffer GetD3D() { return m_buffer; }

	private:		
		BufferUsageType m_bufferType;
		VkBuffer m_buffer;
		VmaAllocation m_allocation;
		VkDescriptorBufferInfo m_descriptor;

		uint32_t m_rendererId;
		size_t m_allocatedSize;
		size_t m_usedSize;
		UINT8* m_mappingAddress;
	};
}

