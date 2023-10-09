#include "directx_fence.h"
#include "directx_graphics_context.h"
#include "directx_copy_context.h"
namespace KNR
{
	DirectXFence::DirectXFence(bool copy)
	{
		ID3D12Device* device = DirectXContext.GetDevice();
		HRESULT hr;

		//Create a fence for GPU synchronization
		hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_fence);
		if (FAILED(hr))
		{
			assert(0);
		}

		//Initialize the starting fence value
		m_fenceValue = 0;

		//Create an event object for the fence.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			//return false;
		}

		m_copyQueue = copy;
	}

	DirectXFence::~DirectXFence()
	{
		m_fence->Release();
	}

	void DirectXFence::IncrementFenceValue()
	{
		m_fenceValue++;
	}

	void DirectXFence::Signal()
	{
		HRESULT hr;
		ID3D12CommandQueue* commandQueue;
		
		commandQueue = DirectXContext.GetCommandQueue();
				 
		hr = commandQueue->Signal(m_fence, m_fenceValue);
		if (FAILED(hr))
		{
			assert(0);
		}
	}

	void DirectXFence::Signal(ID3D12CommandQueue* queue)
	{
		HRESULT hr = queue->Signal(m_fence, m_fenceValue);
		if (FAILED(hr))
		{
			assert(0);
		}
	}

	void DirectXFence::WaitForFenceEvent()
	{
		HRESULT hr;
		//if the current fance value is less than fence value, we know GPU is not finished executing
		if (m_fence->GetCompletedValue() < m_fenceValue)
		{
			//We have the fence create an event which is signaled once the fence's current value is fence value
			hr = m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
			if (FAILED(hr))
			{
				assert(0);
			}

			//we will wait until the fence has triggered the event htat its current value has reached fence value
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}

	void DirectXFence::SetToCurrentFenceValue()
	{
		m_fenceValue = m_fence->GetCompletedValue();
	}
}
