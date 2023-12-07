#include "directx12_buffer.h"
#include "directx12_graphics_context.h"
#include "directx12_renderer_api.h"
#include "directx12_heap.h"
#include "directx12_texture.h"
#include "directx12_technique.h"
#include "directx12_indirect_signature.h"
#include "logger/logger.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <basetsd.h>
#pragma comment(lib, "d3dcompiler")

namespace KNR
{
	DirectX12RendererAPI::~DirectX12RendererAPI()
	{
		for (int i = 0; i < 3; ++i)
		{
			if (m_fences[i])
			{
				delete m_fences[i];
				m_fences[i] = 0;
			}
		}
	}

	void DirectX12RendererAPI::Initialize(const WindowDesc& windowDesc)
	{
		m_rtvHeapIndex = 0;
		for (int i = 0; i < 3; ++i)
		{
			m_fences[i] = new DirectX12Fence(false);
		}
		CreateCPUHeaps();
		CreateRenderTargets();

		m_width = 1920;
		m_height = 1080;
	}

	void DirectX12RendererAPI::SetClearColor(const float r, const float g, const float b, const float a)
	{

	}

	void DirectX12RendererAPI::Clear() const
	{

	}

	void DirectX12RendererAPI::BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		directXCommandList->Get()->IASetVertexBuffers(0, 1, reinterpret_cast<DirectX12Buffer*>(buffer)->GetD3DView<D3D12_VERTEX_BUFFER_VIEW*>());
	}

	void DirectX12RendererAPI::BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		directXCommandList->Get()->IASetIndexBuffer(reinterpret_cast<DirectX12Buffer*>(buffer)->GetD3DView<D3D12_INDEX_BUFFER_VIEW*>());
	}

	void DirectX12RendererAPI::BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		SetConstantBufferView(commandList, bindslot, reinterpret_cast<DirectX12Buffer*>(buffer)->GetD3D()->GetGPUVirtualAddress());
	}

	void DirectX12RendererAPI::BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		SetShaderResourceView(commandList, bindslot, reinterpret_cast<DirectX12Buffer*>(buffer)->GetD3D()->GetGPUVirtualAddress());
	}

	void DirectX12RendererAPI::DrawIndexedInstanced(CommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		directXCommandList->Get()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, baseVertexLocation, startInstanceLocation);
	}

	void DirectX12RendererAPI::DrawIndirect(CommandBuffer* commandList, DirectX12IndirectSignature* DirectX12IndirectSignature, UINT commandCount, Buffer* argumentBuffer, UINT64 argumentBufferOffset, KNR::Buffer* countBuffer, UINT64 countBufferOffset)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		directXCommandList->Get()->ExecuteIndirect(DirectX12IndirectSignature->GetCommandSignature(), commandCount, reinterpret_cast<DirectX12Buffer*>(argumentBuffer)->GetD3D(), argumentBufferOffset, nullptr, countBufferOffset);
	}

	void DirectX12RendererAPI::DispatchCompute(CommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ)
	{

	}

	void DirectX12RendererAPI::SetViewport(const float x, const float y, const float width, const float height)
	{
		//This should never be hit but lets make sure this doesn't happen anyway
		assert(width > 0);
		assert(height > 0);

		m_width = width;
		m_height = height;
		m_viewport.TopLeftX = x;
		m_viewport.TopLeftY = y;
		m_viewport.Width = width;
		m_viewport.Height = height;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_scissorRect.left = 0;
		m_scissorRect.top = 0;
		m_scissorRect.right = width;
		m_scissorRect.bottom = height;

		//Wait for GPU
		WaitForGPU();

		//Kill all resources holding references to the swapchain
		for (int i = 0; i < 3; ++i)
		{
			m_backBufferRenderTarget[i].Reset();
			m_fences[i]->SetToCurrentFenceValue();
		}

		DXGI_SWAP_CHAIN_DESC desc = {};
		IDXGISwapChain3* swapchain = DirectX12Context.GetSwapchain();
		ID3D12Device* device = DirectX12Context.GetDevice();
		swapchain->GetDesc(&desc);
		HRESULT hr = swapchain->ResizeBuffers(3, width, height, desc.BufferDesc.Format, desc.Flags);
		if (FAILED(hr))
		{
			assert(0);
		}

		m_bufferIndex = swapchain->GetCurrentBackBufferIndex();

		CreateRenderTargets();
	}

	void DirectX12RendererAPI::RecordCommandBuffers()
	{
		WaitForPreviousFrame();
		ID3D12Device* device = DirectX12Context.GetDevice();
		ID3D12GraphicsCommandList* m_commandList = DirectX12Context.GetCommandList();
		ID3D12CommandAllocator* m_commandAllocator = DirectX12Context.GetAllocator(m_bufferIndex);
		HRESULT hr;

		hr = m_commandAllocator->Reset();
		if (FAILED(hr))
		{
			assert(0);
		}

		hr = m_commandList->Reset(m_commandAllocator, NULL);
		if (FAILED(hr))
		{
			assert(0);
		}
	}

	//Geometry render begin
	void DirectX12RendererAPI::BeginRender()
	{
		ID3D12CommandQueue* copyQueue = static_cast<ID3D12CommandQueue*>(DirectX12Context.GetCommandQueue());
		DirectX12CommandBuffer* copyCommandBuffer = DirectX12Context.GetCopyCommandBuffer();
		//Is there any copy work to be submitted at the start of the frame
		if (copyCommandBuffer->GetWorkToBeSubmitted())
		{
			copyCommandBuffer->Close();
			copyCommandBuffer->Submit(copyQueue);
			copyCommandBuffer->Wait();
		}

		RecordCommandBuffers();
	}

	//Geometry render end
	void DirectX12RendererAPI::EndRender()
	{
		ID3D12GraphicsCommandList* m_commandList = DirectX12Context.GetCommandList();
		ID3D12CommandQueue* m_commandQueue = DirectX12Context.GetCommandQueue();

		HRESULT hr = m_commandList->Close();
		if (FAILED(hr))
		{
			assert(0);
		}

		ID3D12CommandList* ppCommandLists[] = { m_commandList };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	void DirectX12RendererAPI::Present()
	{
		if (m_imguiEnabled)
		{
			ID3D12GraphicsCommandList* m_commandList = DirectX12Context.GetImguiCommandList();
			ID3D12CommandQueue* m_commandQueue = DirectX12Context.GetCommandQueue();
			auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTarget[m_bufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			m_commandList->ResourceBarrier(1, &transition);

			HRESULT hr = m_commandList->Close();
			if (FAILED(hr))
			{
				assert(0);
			}

			ID3D12CommandList* ppCommandLists[] = { m_commandList };
			m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}

		IDXGISwapChain3* m_swapChain = DirectX12Context.GetSwapchain();
		HRESULT hr = m_swapChain->Present(0, 0);
		if (FAILED(hr))
		{
			assert(0);
		}
	}


	void DirectX12RendererAPI::SetSwapchainRenderTarget()
	{
		ID3D12Device* device = DirectX12Context.GetDevice();
		ID3D12GraphicsCommandList* commandList = m_imguiEnabled ? DirectX12Context.GetImguiCommandList() : DirectX12Context.GetCommandList();

		auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTarget[m_bufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ResourceBarrier(1, &transition);

		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = m_width;
		m_viewport.Height = m_height;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_scissorRect.left = 0;
		m_scissorRect.top = 0;
		m_scissorRect.right = m_width;
		m_scissorRect.bottom = m_height;

		if (m_viewport.Width <= 0)
		{
			KNT_ERROR("ERROR: WIDTH CANNOT BE 0 OR NEGATIVE")
		}
		if (m_scissorRect.right <= 0)
		{
			KNT_ERROR("ERROR: SCISSOR CANNOT BE 0 OR NEGATIVE")
		}

		const float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };

		//We dont want to clear the render target here, only alter what it currently contains
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeap.handleCPU(m_bufferIndex);

		commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
		commandList->OMSetRenderTargets(1, &handle, FALSE, NULL);

		//Deferred G-Buffer Pass
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // IA = Input assembler
		commandList->RSSetViewports(1, &m_viewport);				//RS = Rasterizer
		commandList->RSSetScissorRects(1, &m_scissorRect);
	}

	void DirectX12RendererAPI::SetFinalRenderTextureId(void* textureId)
	{
		m_framebuffer = textureId;
	}



	void* DirectX12RendererAPI::GetFinalRenderTextureId()
	{
		return m_framebuffer;
	}

	void DirectX12RendererAPI::WaitForGPU()
	{
		for (int i = 0; i < 3; ++i)
		{
			m_fences[i]->Signal();
			m_fences[i]->WaitForFenceEvent();
			m_fences[i]->IncrementFenceValue();
		}
	}

	void DirectX12RendererAPI::SetRootConstant(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		if (directXCommandList->GetType() == CommandBufferType::graphics)
		{
			directXCommandList->Get()->SetGraphicsRoot32BitConstant(rootParameterIndex, srcData, destOffsetIn32BitValues);
		}
		else if (directXCommandList->GetType() == CommandBufferType::compute)
		{
			directXCommandList->Get()->SetComputeRoot32BitConstant(rootParameterIndex, srcData, destOffsetIn32BitValues);
		}
	}

	void DirectX12RendererAPI::SetRootConstants(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		if (directXCommandList->GetType() == CommandBufferType::graphics)
		{
			directXCommandList->Get()->SetGraphicsRoot32BitConstants(rootParameterIndex, numValuesSet, srcData, destOffsetIn32BitValues);
		}
		else if (directXCommandList->GetType() == CommandBufferType::compute)
		{
			directXCommandList->Get()->SetComputeRoot32BitConstants(rootParameterIndex, numValuesSet, srcData, destOffsetIn32BitValues);
		}
	}

	void DirectX12RendererAPI::SetConstantBufferView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		if (directXCommandList->GetType() == CommandBufferType::Graphics)
		{
			directXCommandList->Get()->SetGraphicsRootConstantBufferView(bindSlot, gpuAddress);
		}
		else if (commandList->GetType() == CommandBufferType::Compute)
		{
			directXCommandList->Get()->SetComputeRootConstantBufferView(bindSlot, gpuAddress);
		}
	}

	void DirectX12RendererAPI::SetShaderResourceView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		if (directXCommandList->GetType() == CommandBufferType::Graphics)
		{
			directXCommandList->Get()->SetGraphicsRootShaderResourceView(bindSlot, gpuAddress);
		}
		else if (commandList->GetType() == CommandBufferType::Compute)
		{
			directXCommandList->Get()->SetComputeRootShaderResourceView(bindSlot, gpuAddress);
		}
	}

	void DirectX12RendererAPI::SetUnorderedAccessView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		if (directXCommandList->GetType() == CommandBufferType::Graphics)
		{
			directXCommandList->Get()->SetGraphicsRootUnorderedAccessView(bindSlot, gpuAddress);
		}
		else if (directXCommandList->GetType() == CommandBufferType::Compute)
		{
			directXCommandList->Get()->SetComputeRootUnorderedAccessView(bindSlot, gpuAddress);
		}
	}

	void DirectX12RendererAPI::SetRootDescriptorTable(CommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		if (directXCommandList->GetType() == CommandBufferType::Graphics)
		{
			directXCommandList->Get()->SetGraphicsRootDescriptorTable(bindSlot, static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>(startGPUAddress));
		}
		else if (directXCommandList->GetType() == CommandBufferType::Compute)
		{
			directXCommandList->Get()->SetComputeRootDescriptorTable(bindSlot, static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>(startGPUAddress));
		}
	}

	void DirectX12RendererAPI::WaitForPreviousFrame()
	{
		IDXGISwapChain3* swapchain = DirectX12Context.GetSwapchain();
		//wait for the previous frame to compile
		m_bufferIndex = swapchain->GetCurrentBackBufferIndex();
		m_fences[m_bufferIndex]->Signal();
		m_fences[m_bufferIndex]->WaitForFenceEvent();
		m_fences[m_bufferIndex]->IncrementFenceValue();
	}

	void DirectX12RendererAPI::CreateCPUHeaps()
	{
		//Liam Fix - Load the frameheap (Only do this one, we can deal with safely doing this later)
		m_rtvHeap.Create(DirectX12Context.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3); //unbound texture heap
	}

	void DirectX12RendererAPI::CreateRenderTargets()
	{
		IDXGISwapChain3* swapchain = DirectX12Context.GetSwapchain();
		m_rtvHeapIndex = 0;
		
		//Loop for each frame in flight
		for (int i = 0; i < 3; ++i)
		{
			HRESULT result;
			//Get a pointer to the first back buffer from the swapchain
			result = swapchain->GetBuffer(i, IID_PPV_ARGS(&m_backBufferRenderTarget[i]));
			if (FAILED(result))
			{
				assert(0);
			}
			m_backBufferRenderTarget[i]->SetName(L"Swapchain Back Buffer");
			DirectX12Context.GetDevice()->CreateRenderTargetView(m_backBufferRenderTarget[i].Get(), NULL, m_rtvHeap.handleCPU(m_rtvHeapIndex));
			m_rtvHeapIndex++;
		}
	}
}