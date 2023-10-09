#pragma once
#include <cstdint>
#include "buffer.h"
#include "directx_graphics_context.h"
#include <d3d12.h>
namespace KNR
{
	class DirectXBuffer
		: public Buffer
	{
	public:
		DirectXBuffer(const BufferDescriptor& desc);
		~DirectXBuffer();

		virtual void Bind(DirectXCommandBuffer* commandList) const override;
		virtual void Unbind(DirectXCommandBuffer* commandList) const override;

		virtual void* Map() override;
		virtual void UnMap() override;

		virtual size_t GetCapacitySize() override { return m_allocatedSize; }
		virtual size_t GetUsedSize() override { return m_usedSize; }

		virtual uint32_t AppendData(uint32_t size, void* data) override;
		ID3D12Resource* GetD3D() { return m_resource; }
		
		DirectXDescriptorHandleBlock GetDescriptorBlock() { return m_descriptorHandleBlock; }

		inline static void Destroy(ID3D12Resource* uploadBuffer) { uploadBuffer->Release(); uploadBuffer = 0; };

		template<class T>
		T GetD3DView();

	private:
		DirectXDescriptorHandleBlock m_descriptorHandleBlock;
		uint32_t m_cbvHeapIndex;
		
		union {
			D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
			D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
			D3D12_CONSTANT_BUFFER_VIEW_DESC m_constantBufferView;
			D3D12_UNORDERED_ACCESS_VIEW_DESC m_unorderedBufferView;
			D3D12_SHADER_RESOURCE_VIEW_DESC m_structuredBufferView;
		};
		BufferUsageType m_bufferType;
		ID3D12Resource* m_resource;
		uint32_t m_rendererId;
		size_t m_allocatedSize;
		size_t m_usedSize;
		UINT8* m_mappingAddress;
	};

	template<> inline D3D12_VERTEX_BUFFER_VIEW* DirectXBuffer::GetD3DView()
	{
		return &m_vertexBufferView;
	}

	template<> inline D3D12_INDEX_BUFFER_VIEW* DirectXBuffer::GetD3DView()
	{
		return &m_indexBufferView;
	}

	template<> inline D3D12_CONSTANT_BUFFER_VIEW_DESC* DirectXBuffer::GetD3DView()
	{
		return &m_constantBufferView;
	}

	template<> inline D3D12_UNORDERED_ACCESS_VIEW_DESC* DirectXBuffer::GetD3DView()
	{
		return &m_unorderedBufferView;
	}
	
	template<> inline D3D12_SHADER_RESOURCE_VIEW_DESC* DirectXBuffer::GetD3DView()
	{
		return &m_structuredBufferView;
	}
}
