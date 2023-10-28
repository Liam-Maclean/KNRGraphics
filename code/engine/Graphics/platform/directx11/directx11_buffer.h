#pragma once
#include <cstdint>
#include "buffer.h"
namespace KNR
{
	class DirectX11Buffer
		: public Buffer
	{
	public:
		DirectX11Buffer(const BufferDescriptor& desc);
		~DirectX11Buffer();

		virtual void Bind(CommandBuffer* commandList) const override;
		virtual void Unbind(CommandBuffer* commandList) const override;

		virtual void* Map() override;
		virtual void UnMap() override;

		virtual size_t GetCapacitySize() override { return m_allocatedSize; }
		virtual size_t GetUsedSize() override { return m_usedSize; }

		virtual uint32_t AppendData(uint32_t size, void* data) override;

	private:		
		BufferUsageType m_bufferType;

		uint32_t m_rendererId;
		size_t m_allocatedSize;
		size_t m_usedSize;
		UINT8* m_mappingAddress;
	};
}

