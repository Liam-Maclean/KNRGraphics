#include "directx_graphics_context.h"
#include "d3dx12.h"
#include "directx_copy_context.h"
#include <d3dcompiler.h>
#include <d3d12.h>
#include <dxgi1_5.h>

#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d12")

#define DX12_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_1

namespace KNR
{
	CDirectXCopyContext::CDirectXCopyContext()
	{
		CreateQueues();
		CreateCommandList();

		m_copyFence = new DirectXFence(true);
	}

	CDirectXCopyContext::~CDirectXCopyContext()
	{
		m_copyQueue->Release();
		m_copyCommandAllocator->Release();
		m_copyCommandList->Release();
	}

	void CDirectXCopyContext::CreateQueues()
	{
		D3D12_COMMAND_QUEUE_DESC copyQueueDesc;
		//Initialize the description of the command queue
		ZeroMemory(&copyQueueDesc, sizeof(copyQueueDesc));
		//Set up the description of the command queue.
		copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		copyQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		copyQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		copyQueueDesc.NodeMask = 0;

		HRESULT result = DirectXContext.GetDevice()->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&m_copyQueue));
		if (FAILED(result))
		{
			assert(0);
		}
	}

	void CDirectXCopyContext::CreateCommandList()
	{
		ID3D12Device* device = DirectXContext.GetDevice();
		HRESULT result;

		//Create copy command allocator
		result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_copyCommandAllocator));
		if (FAILED(result))
		{
			assert(0);
		}

		//Create the command list from it 
		result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_copyCommandAllocator, nullptr, IID_PPV_ARGS(&m_copyCommandList));
		if (FAILED(result))
		{
			assert(0);
		}

		result = m_copyCommandList->Close();
		if (FAILED(result))
		{
			assert(0);
		}
	}


	void CDirectXCopyContext::StartCopyTextureRegion(ID3D12Resource* srcResource, ID3D12Resource* dstResource, int width, int height)
	{
		assert(width >= 0);
		assert(height >= 0);
		m_copyCommandAllocator->Reset();
		m_copyCommandList->Reset(m_copyCommandAllocator, nullptr);
		CD3DX12_TEXTURE_COPY_LOCATION dest(dstResource);
		CD3DX12_TEXTURE_COPY_LOCATION src(srcResource);

		auto preDstBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dstResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
		auto preSrcBarrier = CD3DX12_RESOURCE_BARRIER::Transition(srcResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE);


		m_copyCommandList->ResourceBarrier(1, &preDstBarrier);
		m_copyCommandList->ResourceBarrier(1, &preSrcBarrier);
		CD3DX12_BOX box(0, 0, width, height);
		m_copyCommandList->CopyTextureRegion(&dest, 0, 0, 0, &src, &box);

		auto postDstBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dstResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		auto postSrcBarrier = CD3DX12_RESOURCE_BARRIER::Transition(srcResource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_copyCommandList->ResourceBarrier(1, &postDstBarrier);
		m_copyCommandList->ResourceBarrier(1, &postSrcBarrier);
	}

	void CDirectXCopyContext::EndCopyTextureRegion()
	{
		ID3D12CommandList* ppCommandLists[] = { m_copyCommandList };
		m_copyQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		m_copyFence->Signal();
		WaitAndReset();
	}

	void CDirectXCopyContext::StartUpload(ID3D12Resource* resource, size_t bufferSize, void* data)
	{
		assert(data != nullptr);
		assert(bufferSize >= 0);
		m_copyCommandAllocator->Reset();
		m_copyCommandList->Reset(m_copyCommandAllocator, nullptr);

		auto cHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto cResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		HRESULT hr = DirectXContext.GetDevice()->CreateCommittedResource(&cHeapProperties, D3D12_HEAP_FLAG_NONE, &cResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_uploadResource));
		if (FAILED(hr))
		{
			assert(0);
		}

		const D3D12_RANGE range{};
		m_uploadResource->Map(0, &range, reinterpret_cast<void**>(&m_mappingAddress));
		memcpy(m_mappingAddress, data, bufferSize);
		m_copyCommandList->CopyResource(resource, m_uploadResource);
	}

	void CDirectXCopyContext::EndUpload()
	{
		ID3D12CommandList* ppCommandLists[] = { m_copyCommandList };
		m_copyQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		m_copyFence->Signal();
		WaitAndReset();
	}

	void CDirectXCopyContext::WaitAndReset()
	{
		m_copyFence->WaitForFenceEvent();
		if (m_uploadResource)
		{
			m_uploadResource->Release();
			m_uploadResource = 0;
		}
	}
}