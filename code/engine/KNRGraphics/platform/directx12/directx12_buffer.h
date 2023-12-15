#pragma once
#include <cstdint>
#include "buffer.h"
#include "directx12_graphics_context.h"
#include <d3d12.h>
namespace KNR
{
	class DirectX12Buffer final
		: public Buffer
	{
	public:
		DirectX12Buffer(const BufferDescriptor& desc);

		~DirectX12Buffer();

		void* Map() override;

		void UnMap() override;

		//Getters/Setters
		inline size_t GetCapacitySize() const override { return m_allocatedSize; }

		inline size_t GetUsedSize() const override { return m_usedSize; }

		inline ID3D12Resource* GetD3D() const { return m_resource; }

		inline DirectX12DescriptorHandleBlock GetDescriptorBlock() const { return m_descriptorHandleBlock; }

		inline static void Destroy(ID3D12Resource* uploadBuffer) { uploadBuffer->Release(); uploadBuffer = 0; };

		template<class T>
		T GetD3DView();

	private:
		DirectX12DescriptorHandleBlock m_descriptorHandleBlock;
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

	template<> inline D3D12_VERTEX_BUFFER_VIEW* DirectX12Buffer::GetD3DView()
	{
		return &m_vertexBufferView;
	}

	template<> inline D3D12_INDEX_BUFFER_VIEW* DirectX12Buffer::GetD3DView()
	{
		return &m_indexBufferView;
	}

	template<> inline D3D12_CONSTANT_BUFFER_VIEW_DESC* DirectX12Buffer::GetD3DView()
	{
		return &m_constantBufferView;
	}

	template<> inline D3D12_UNORDERED_ACCESS_VIEW_DESC* DirectX12Buffer::GetD3DView()
	{
		return &m_unorderedBufferView;
	}
	
	template<> inline D3D12_SHADER_RESOURCE_VIEW_DESC* DirectX12Buffer::GetD3DView()
	{
		return &m_structuredBufferView;
	}
}

