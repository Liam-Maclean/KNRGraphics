#include "directx12_command_buffer.h"
#include "directx12_graphics_context.h"
#include "render_types.h"
#include <d3d12.h>
#include "d3dx12.h"

namespace KNR
{
	DirectX12CommandBuffer::DirectX12CommandBuffer(CommandBufferType type)
	{
		ID3D12Device* device = DirectX12Context.GetDevice();
		D3D12_COMMAND_LIST_TYPE commandListType;
		m_type = type;
		HRESULT hr;
		switch (m_type)
		{
		case CommandBufferType::compute:
			m_fence = new DirectX12Fence(false);
			commandListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		case CommandBufferType::copy:
			m_fence = new DirectX12Fence(true);
			commandListType = D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		case CommandBufferType::graphics:
			m_fence = new DirectX12Fence(false);
			commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		default:
			break;
		}

		//Create an allocator
		hr = device->CreateCommandAllocator(commandListType, __uuidof(ID3D12CommandAllocator), (void**)&m_commandAllocator);
		if (FAILED(hr))
		{
			assert(0);
		}

		//Create a command list
		hr = device->CreateCommandList(0, commandListType, m_commandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_commandList);
		if (FAILED(hr))
		{
			assert(0);
		}
		m_workToBeSubmitted = true;
	}

	DirectX12CommandBuffer::~DirectX12CommandBuffer()
	{
		m_commandList->Release();
		m_commandList = 0;

		m_commandAllocator->Release();
		m_commandAllocator = 0;

		delete m_fence;
		m_fence = 0;
	}

	void DirectX12CommandBuffer::Reset()
	{
		assert(!m_workToBeSubmitted);
		m_commandAllocator->Reset();
		m_commandList->Reset(m_commandAllocator, nullptr);
		m_workToBeSubmitted = true;
	}

	void DirectX12CommandBuffer::Reset(ID3D12CommandAllocator* customAllocator)
	{
		assert(!m_workToBeSubmitted);
		customAllocator->Reset();
		m_commandList->Reset(customAllocator, nullptr);
		m_workToBeSubmitted = true;
	}

	void DirectX12CommandBuffer::Close()
	{
		assert(m_workToBeSubmitted);
		m_commandList->Close();
		m_workToBeSubmitted = false;
	}

	void DirectX12CommandBuffer::Submit(ID3D12CommandQueue* queue)
	{
		ID3D12CommandList* ppCommandLists[] = { m_commandList };
		queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);	
		m_fence->IncrementFenceValue();
		m_fence->Signal(queue);
	}

	void DirectX12CommandBuffer::SubmitWorkImmediate()
	{
		ID3D12CommandQueue* queue;
		switch (m_type)
		{
		case CommandBufferType::copy:
			queue = static_cast<ID3D12CommandQueue*>(DirectX12Context.GetCommandQueue());
			break;
		case CommandBufferType::compute:
			queue = static_cast<ID3D12CommandQueue*>(DirectX12Context.GetComputeQueue());
			break;
		case CommandBufferType::graphics:
			queue = static_cast<ID3D12CommandQueue*>(DirectX12Context.GetCommandQueue());
			break;
		default:
			break;
		}

		Close();
		Submit(queue);
		Wait();
	}

	void DirectX12CommandBuffer::Wait()
	{
		m_fence->WaitForFenceEvent();

		//If we have any callbacks to call, we've finished executing on the gpu and now is the time
		if (!m_commandBufferCallbackList.IsEmpty())
		{
			m_commandBufferCallbackList.ReverseInvoke();
			m_commandBufferCallbackList.Clear();
		}
	}

	void DirectX12CommandBuffer::UpdateCopyResource(ID3D12Resource* dstResource, ID3D12Resource* srcResource, D3D12_SUBRESOURCE_DATA subresources)
	{
		if (!m_workToBeSubmitted)
		{
			Reset();
		}

		UpdateSubresources(m_commandList, dstResource, srcResource, 0, 0, 1, &subresources);
	}

	void DirectX12CommandBuffer::UpdateCopyResource(ID3D12Resource* dstResource, ID3D12Resource* srcResource, std::vector<D3D12_SUBRESOURCE_DATA> subresources)
	{
		if (!m_workToBeSubmitted)
		{
			Reset();
		}

		UpdateSubresources(m_commandList, dstResource, srcResource, 0, 0, 1, subresources.data());
	}


	void DirectX12CommandBuffer::CopyResource(ID3D12Resource* dstResource, ID3D12Resource* srcResource)
	{
		if (!m_workToBeSubmitted)
		{
			Reset();
		}

		m_commandList->CopyResource(dstResource, srcResource);
	}

	void DirectX12CommandBuffer::AddToCommandCallbackList(Delegate<>::Func_type&& func)
	{
		m_commandBufferCallbackList += std::forward<Delegate<>::Func_type>(func);
	}

}


	


