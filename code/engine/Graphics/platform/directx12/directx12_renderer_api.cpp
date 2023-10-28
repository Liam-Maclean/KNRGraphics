#include "directx12_buffer.h"
#include "directx12_graphics_context.h"
#include "directx12_copy_context.h"
#include "directx12_renderer_api.h"
#include "directx12_heap.h"
#include "directx12_texture.h"
#include "directx12_technique.h"
#include "directx12_indirect_signature.h"
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

	void DirectX12RendererAPI::Init()
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

	void DirectX12RendererAPI::SetClearColor(const Vector4f& color)
	{

	}

	void DirectX12RendererAPI::Clear() const
	{

	}

	uint32_t DirectX12RendererAPI::AppendBufferRegion(DirectX12CommandBuffer* commandList, KNR::Buffer* dstBuffer, KNR::Buffer* srcBuffer)
	{
		uint32_t destUsedSize = dstBuffer->GetUsedSize();
		uint32_t destMaxCapacity = dstBuffer->GetCapacitySize();
		uint32_t srcCopySize = srcBuffer->GetUsedSize();
		uint32_t combinedSize = destUsedSize + srcCopySize;

		if (combinedSize > destMaxCapacity)
		{
			//Exceeded capacity
			assert(0);
			return 0;
		}
		else
		{
			commandList->Get()->CopyBufferRegion(reinterpret_cast<DirectX12Buffer*>(dstBuffer)->GetD3D(), destUsedSize, reinterpret_cast<DirectX12Buffer*>(dstBuffer)->GetD3D(), 0, srcCopySize);
			return combinedSize;
		}
	}

	void DirectX12RendererAPI::BindPipeline(DirectX12CommandBuffer* commandList, KNR::Technique* technique)
	{
		//Bind as normal
		technique->Bind(commandList);

		////We also need bind any bindless arrays. Best way to do this is access a global heap and it's GPU handle
		for (int i = (int)BindlessHeapRegion::BINDLESSSTART; i < (int)BindlessHeapRegion::BINDLESSEND; i++)
		{
			//We need to source the handle from the global heap object
			//D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle;
			//commandList->Get()->SetGraphicsRootDescriptorTable(i, descriptorHandle);
		}
	}

	void DirectX12RendererAPI::BindVertexBuffer(DirectX12CommandBuffer* commandList, Buffer* buffer)
	{
		commandList->Get()->IASetVertexBuffers(0, 1, reinterpret_cast<DirectX12Buffer*>(buffer)->GetD3DView<D3D12_VERTEX_BUFFER_VIEW*>());
	}

	void DirectX12RendererAPI::BindIndexBuffer(DirectX12CommandBuffer* commandList, Buffer* buffer)
	{
		commandList->Get()->IASetIndexBuffer(reinterpret_cast<DirectX12Buffer*>(buffer)->GetD3DView<D3D12_INDEX_BUFFER_VIEW*>());
	}

	void DirectX12RendererAPI::BindUniformBuffer(DirectX12CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		SetConstantBufferView(commandList, bindslot, reinterpret_cast<DirectX12Buffer*>(buffer)->GetD3D()->GetGPUVirtualAddress());
	}

	void DirectX12RendererAPI::BindStructuredBuffer(DirectX12CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		SetShaderResourceView(commandList, bindslot, reinterpret_cast<DirectX12Buffer*>(buffer)->GetD3D()->GetGPUVirtualAddress());
	}

	void DirectX12RendererAPI::DrawIndexedInstanced(DirectX12CommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
	{
		commandList->Get()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, baseVertexLocation, startInstanceLocation);
	}

	void DirectX12RendererAPI::DrawIndirect(DirectX12CommandBuffer* commandList, DirectX12IndirectSignature* DirectX12IndirectSignature, UINT commandCount, Buffer* argumentBuffer, UINT64 argumentBufferOffset, KNR::Buffer* countBuffer, UINT64 countBufferOffset)
	{
		commandList->Get()->ExecuteIndirect(DirectX12IndirectSignature->GetCommandSignature(), commandCount, reinterpret_cast<DirectX12Buffer*>(argumentBuffer)->GetD3D(), argumentBufferOffset, nullptr, countBufferOffset);
	}

	void DirectX12RendererAPI::DispatchCompute(DirectX12CommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ)
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

	void DirectX12RendererAPI::SetWireframeMode(int i)
	{

	}

	void DirectX12RendererAPI::PopState()
	{

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


	void DirectX12RendererAPI::BeginRenderSilent()
	{

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
			//Error
		}
		if (m_scissorRect.right <= 0)
		{
			//Error
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

	void DirectX12RendererAPI::BlitToTexture(Texture2D* srcTx, Texture2D* dstTx)
	{
		DirectX12Context.StartBlit(reinterpret_cast<DirectX12Texture2D*>(srcTx)->GetTextureHandle(), reinterpret_cast<DirectX12Texture2D*>(dstTx)->GetTextureHandle(), srcTx->GetWidth(), srcTx->GetHeight());
		DirectX12Context.EndBlit();
	}

	void DirectX12RendererAPI::BlitToSwapchain(Texture2D* srcTx)
	{
		DirectX12Context.StartBlitToSwapchain(reinterpret_cast<DirectX12Texture2D*>(srcTx)->GetTextureHandle(), m_backBufferRenderTarget[m_bufferIndex].Get(), srcTx->GetWidth(), srcTx->GetHeight());
		DirectX12Context.EndBlit();
	}

	void DirectX12RendererAPI::SetRootConstant(DirectX12CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues)
	{
		if (commandList->GetType() == CommandBufferType::graphics)
		{
			commandList->Get()->SetGraphicsRoot32BitConstant(rootParameterIndex, srcData, destOffsetIn32BitValues);
		}
		else if (commandList->GetType() == CommandBufferType::compute)
		{
			commandList->Get()->SetComputeRoot32BitConstant(rootParameterIndex, srcData, destOffsetIn32BitValues);
		}
	}

	void DirectX12RendererAPI::SetRootConstants(DirectX12CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues)
	{
		if (commandList->GetType() == CommandBufferType::graphics)
		{
			commandList->Get()->SetGraphicsRoot32BitConstants(rootParameterIndex, numValuesSet, srcData, destOffsetIn32BitValues);
		}
		else if (commandList->GetType() == CommandBufferType::compute)
		{
			commandList->Get()->SetComputeRoot32BitConstants(rootParameterIndex, numValuesSet, srcData, destOffsetIn32BitValues);
		}
	}

	void DirectX12RendererAPI::SetConstantBufferView(DirectX12CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		if (commandList->GetType() == CommandBufferType::graphics)
		{
			commandList->Get()->SetGraphicsRootConstantBufferView(bindSlot, gpuAddress);
		}
		else if (commandList->GetType() == CommandBufferType::compute)
		{
			commandList->Get()->SetComputeRootConstantBufferView(bindSlot, gpuAddress);
		}
	}

	void DirectX12RendererAPI::SetShaderResourceView(DirectX12CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		if (commandList->GetType() == CommandBufferType::graphics)
		{
			commandList->Get()->SetGraphicsRootShaderResourceView(bindSlot, gpuAddress);
		}
		else if (commandList->GetType() == CommandBufferType::compute)
		{
			commandList->Get()->SetComputeRootShaderResourceView(bindSlot, gpuAddress);
		}
	}

	void DirectX12RendererAPI::SetUnorderedAccessView(DirectX12CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		if (commandList->GetType() == CommandBufferType::graphics)
		{
			commandList->Get()->SetGraphicsRootUnorderedAccessView(bindSlot, gpuAddress);
		}
		else if (commandList->GetType() == CommandBufferType::compute)
		{
			commandList->Get()->SetComputeRootUnorderedAccessView(bindSlot, gpuAddress);
		}
	}

	void DirectX12RendererAPI::SetRootDescriptorTable(DirectX12CommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress)
	{
		if (commandList->GetType() == CommandBufferType::graphics)
		{
			commandList->Get()->SetGraphicsRootDescriptorTable(bindSlot, static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>(startGPUAddress));
		}
		else if (commandList->GetType() == CommandBufferType::compute)
		{
			commandList->Get()->SetComputeRootDescriptorTable(bindSlot, static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>(startGPUAddress));
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