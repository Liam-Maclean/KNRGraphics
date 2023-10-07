#include "directx_buffer.h"
#include "directx_graphics_context.h"
#include "directx_copy_context.h"
#include "d3dx12.h"
namespace KNR
{
	Buffer* Buffer::Create(const BufferDescriptor& desc)
	{
		return new DirectXBuffer(desc);
	}

	DirectXBuffer::DirectXBuffer(const BufferDescriptor& desc)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectXContext.GetDevice());
		m_bufferType = desc.bufferType;
		m_allocatedSize = desc.size;
		m_usedSize = 0;
		D3D12_RESOURCE_STATES resourceState;
		CD3DX12_HEAP_PROPERTIES heapProperties;
		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = desc.bufferType == BufferUsageType::UNIFORM ? m_allocatedSize = (desc.size + 255) & ~255 : m_allocatedSize; //Uniform buffers be 256 aligned
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Alignment = 0;

		switch (desc.bufferAccessType)
		{
		case BufferAccessType::DEFAULT:
			heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		case BufferAccessType::UPLOAD:
			heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
			break;
		case BufferAccessType::READBACK:
			heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
			break;
		case BufferAccessType::CUSTOM:
			heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_CUSTOM);
			break;
		default:
			break;
		}

		//CPU resource
		HRESULT hr;
		hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, nullptr, IID_PPV_ARGS(&m_resource));
		if (FAILED(hr))
		{
			assert(0);
		}

		if (desc.debugName)
		{
			m_resource->SetName(desc.debugName);
		}

		//Create the views for the buffer
		if (desc.bufferType == BufferUsageType::VERTEX)
		{
			m_vertexBufferView = {};
			m_vertexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
			m_vertexBufferView.SizeInBytes = m_allocatedSize;
			m_vertexBufferView.StrideInBytes = desc.vertexBuffer.stride;
		}
		else if (desc.bufferType == BufferUsageType::INDEX)
		{
			m_indexBufferView = {};
			m_indexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
			m_indexBufferView.SizeInBytes = m_allocatedSize;
			m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		}
		else if (desc.bufferType == BufferUsageType::UNIFORM)
		{
			m_constantBufferView = {};
			m_constantBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
			m_constantBufferView.SizeInBytes = m_allocatedSize;

			//Liam - Replace with new heap reserved CBV placement
			//frameHeap->CreateCBV(m_constantBufferView, m_descriptorHandleBlock);
		}
		else if (desc.bufferType == BufferUsageType::STORAGE)
		{
			m_structuredBufferView = {};
			m_structuredBufferView.Format = DXGI_FORMAT_UNKNOWN;
			m_structuredBufferView.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			m_structuredBufferView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			m_structuredBufferView.Buffer.FirstElement = 0;
			m_structuredBufferView.Buffer.NumElements = desc.structuredBuffer.numElements;
			m_structuredBufferView.Buffer.StructureByteStride = desc.structuredBuffer.structuredByteStride;
			m_structuredBufferView.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

			//Liam - Replace with new heap reserved SRV placement
			//frameHeap->CreateSRV(m_resource, m_structuredBufferView, m_descriptorHandleBlock);
		}

		//If we have initial data, we need to stage
		if (desc.initialData)
		{
			if (resourceState == D3D12_RESOURCE_STATE_COPY_DEST)
			{
				//We need to stage our data
				DirectXCommandBuffer* copyCommandBuffer = DirectXContext.GetCopyCommandBuffer();
				CD3DX12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				ID3D12Resource* uploadResource;
				hr = DirectXContext.GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadResource));
				if (FAILED(hr))
				{
					assert(0);
				}

				uploadResource->SetName(L"Vertex buffer upload");

				D3D12_RANGE range = { 0, m_allocatedSize };
				void* map;
				uploadResource->Map(0, &range, &map);
				memcpy(map, desc.initialData, m_allocatedSize);
				uploadResource->Unmap(0, nullptr);

				copyCommandBuffer->CopyResource(m_resource, uploadResource);


				auto cResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
				copyCommandBuffer->Get()->ResourceBarrier(1, &cResourceBarrier);
				copyCommandBuffer->AddToCommandCallbackList([uploadResource]() { DirectXBuffer::Destroy(uploadResource); });
			}
			else if (resourceState == D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				//We just need to map our data
				void* map = Map();
				memcpy(map, desc.initialData, m_allocatedSize);
				UnMap();
			}

		}
	}

	DirectXBuffer::~DirectXBuffer()
	{
		m_resource->Release();
		m_resource = nullptr;
	}

	void DirectXBuffer::Bind(DirectXCommandBuffer* commandList) const
	{

	}

	void DirectXBuffer::Unbind(DirectXCommandBuffer* commandList) const
	{

	}

	void* DirectXBuffer::Map()
	{
		D3D12_RANGE range = {0, m_allocatedSize};
		
		void* data;
		HRESULT hr = m_resource->Map(0, &range, &data);
		if (FAILED(hr))
		{
			assert(0);
		}
		return data;
	}

	void DirectXBuffer::UnMap()
	{
		m_resource->Unmap(0, nullptr);
	}

	uint32_t DirectXBuffer::AppendData(uint32_t size, void* data)
	{
		uint32_t combinedSize = size + m_usedSize;
		
		if (combinedSize > m_allocatedSize)
			assert(0); //Exceeded allocated buffer size when appending
		
		D3D12_RANGE newDataRange = { m_usedSize, m_usedSize + size};
		void* memAddr;
		m_resource->Map(0, &newDataRange, &memAddr);
		memcpy(memAddr, data, size);
		m_resource->Unmap(0, nullptr);

		m_usedSize = combinedSize;


		if (m_bufferType == BufferUsageType::VERTEX)
		{
			m_vertexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
			m_vertexBufferView.SizeInBytes = m_usedSize;
		}
		else if (m_bufferType == BufferUsageType::INDEX)
		{
			m_indexBufferView.BufferLocation = m_resource->GetGPUVirtualAddress();
			m_indexBufferView.SizeInBytes = m_usedSize;
		}

		return combinedSize;
	}
}