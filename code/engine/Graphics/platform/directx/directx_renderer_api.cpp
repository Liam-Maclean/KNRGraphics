#include "qlpch.h"

#include "directx_buffer.h"
#include "directx_graphics_context.h"
#include "directx_copy_context.h"
#include "directx_renderer_api.h"
#include "directx_heap.h"
#include "directx_texture.h"
#include "directx_technique.h"
#include "directx_indirect_signature.h"
#include "logging/log.h"
#include <d3dcompiler.h>
#include "d3dx12.h"
#pragma comment(lib, "d3dcompiler")

namespace QRender
{
	DirectXRendererAPI::~DirectXRendererAPI()
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

	void DirectXRendererAPI::Init()
	{
		m_rtvHeapIndex = 0;
		for (int i = 0; i < 3; ++i)
		{
			m_fences[i] = new DirectXFence(false);
		}
		CreateCPUHeaps();
		CreateRenderTargets();

		m_width = 1920;
		m_height = 1080;
	}

	void DirectXRendererAPI::SetClearColor(const Vector4f& color)
	{

	}

	void DirectXRendererAPI::Clear() const
	{

	}

	void DirectXRendererAPI::DrawIndexed(Ref<DirectXCommandBuffer> commandList, const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{

	}

	void DirectXRendererAPI::DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		//vertexArray->Bind();
	}

	void DirectXRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		//vertexArray->Bind();
	}

	uint32_t DirectXRendererAPI::AppendBufferRegion(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> dstBuffer, Ref<QRender::Buffer> srcBuffer)
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
			commandList->Get()->CopyBufferRegion(std::static_pointer_cast<DirectXBuffer>(dstBuffer)->GetD3D(), destUsedSize, std::static_pointer_cast<DirectXBuffer>(dstBuffer)->GetD3D(), 0, srcCopySize);
			return combinedSize;
		}
	}

	void DirectXRendererAPI::BindPipeline(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Technique> technique)
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

	void DirectXRendererAPI::BindVertexBuffer(Ref<DirectXCommandBuffer> commandList, Ref<Buffer> buffer)
	{
		commandList->Get()->IASetVertexBuffers(0, 1, std::static_pointer_cast<DirectXBuffer>(buffer)->GetD3DView<D3D12_VERTEX_BUFFER_VIEW*>());
	}

	void DirectXRendererAPI::BindIndexBuffer(Ref<DirectXCommandBuffer> commandList, Ref<Buffer> buffer)
	{
		commandList->Get()->IASetIndexBuffer(std::static_pointer_cast<DirectXBuffer>(buffer)->GetD3DView<D3D12_INDEX_BUFFER_VIEW*>());
	}

	void DirectXRendererAPI::BindUniformBuffer(Ref<DirectXCommandBuffer> commandList, Ref<Buffer> buffer, uint32_t bindslot)
	{
		SetConstantBufferView(commandList, bindslot, std::static_pointer_cast<DirectXBuffer>(buffer)->GetD3D()->GetGPUVirtualAddress());
	}

	void DirectXRendererAPI::BindStructuredBuffer(Ref<DirectXCommandBuffer> commandList, Ref<Buffer> buffer, uint32_t bindslot)
	{
		SetShaderResourceView(commandList, bindslot, std::static_pointer_cast<DirectXBuffer>(buffer)->GetD3D()->GetGPUVirtualAddress());
	}

	void DirectXRendererAPI::DrawIndexedInstanced(Ref<DirectXCommandBuffer> commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
	{
		commandList->Get()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, baseVertexLocation, startInstanceLocation);
	}

	void DirectXRendererAPI::DrawIndirect(Ref<DirectXCommandBuffer> commandList, Ref<IndirectSignature> indirectSignature, UINT commandCount, Ref<QRender::Buffer> argumentBuffer, UINT64 argumentBufferOffset, Ref<QRender::Buffer> countBuffer, UINT64 countBufferOffset)
	{
		commandList->Get()->ExecuteIndirect(indirectSignature->GetCommandSignature(), commandCount, std::static_pointer_cast<DirectXBuffer>(argumentBuffer)->GetD3D(), argumentBufferOffset, nullptr, countBufferOffset);
	}

	void DirectXRendererAPI::DispatchCompute(Ref<DirectXCommandBuffer> commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ)
	{

	}

	void DirectXRendererAPI::SetViewport(const float x, const float y, const float width, const float height)
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
		IDXGISwapChain3* swapchain = DirectXContext.GetSwapchain();
		ID3D12Device* device = DirectXContext.GetDevice();
		swapchain->GetDesc(&desc);
		HRESULT hr = swapchain->ResizeBuffers(3, width, height, desc.BufferDesc.Format, desc.Flags);
		if (FAILED(hr))
		{
			assert(0);
		}

		m_bufferIndex = swapchain->GetCurrentBackBufferIndex();

		CreateRenderTargets();
	}

	void DirectXRendererAPI::SetWireframeMode(int i)
	{

	}

	void DirectXRendererAPI::PopState()
	{

	}

	void DirectXRendererAPI::RecordCommandBuffers()
	{
		WaitForPreviousFrame();
		ID3D12Device* device = DirectXContext.GetDevice();
		ID3D12GraphicsCommandList* m_commandList = DirectXContext.GetCommandList();
		ID3D12CommandAllocator* m_commandAllocator = DirectXContext.GetAllocator(m_bufferIndex);
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


	void DirectXRendererAPI::BeginRenderSilent()
	{

	}

	//Geometry render begin
	void DirectXRendererAPI::BeginRender()
	{
		ID3D12CommandQueue* copyQueue = static_cast<ID3D12CommandQueue*>(DirectXContext.GetCommandQueue());
		DirectXCommandBuffer* copyCommandBuffer = DirectXContext.GetCopyCommandBuffer();
		//Is there any copy work to be submitted at the start of the frame
		if (copyCommandBuffer->GetWorkToBeSubmitted())
		{
			copyCommandBuffer->Close();
			copyCommandBuffer->Submit(copyQueue);
			copyCommandBuffer->Wait();
			Q_RENDER_INFO("Copy Command Finished");
		}

		RecordCommandBuffers();
	}

	//Geometry render end
	void DirectXRendererAPI::EndRender()
	{
		ID3D12GraphicsCommandList* m_commandList = DirectXContext.GetCommandList();
		ID3D12CommandQueue* m_commandQueue = DirectXContext.GetCommandQueue();

		HRESULT hr = m_commandList->Close();
		if (FAILED(hr))
		{
			assert(0);
		}

		ID3D12CommandList* ppCommandLists[] = { m_commandList };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	void DirectXRendererAPI::Present()
	{
		if (m_imguiEnabled)
		{
			ID3D12GraphicsCommandList* m_commandList = DirectXContext.GetImguiCommandList();
			ID3D12CommandQueue* m_commandQueue = DirectXContext.GetCommandQueue();
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

		IDXGISwapChain3* m_swapChain = DirectXContext.GetSwapchain();
		HRESULT hr = m_swapChain->Present(0, 0);
		if (FAILED(hr))
		{
			assert(0);
		}
	}


	void DirectXRendererAPI::SetSwapchainRenderTarget()
	{
		ID3D12Device* device = DirectXContext.GetDevice();
		ID3D12GraphicsCommandList* commandList = m_imguiEnabled ? DirectXContext.GetImguiCommandList() : DirectXContext.GetCommandList();

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
			Q_RENDER_ERROR("VIEWPORT DIMENSIONS HAVE NOT BEEN SET BEFORE RENDERING, THIS WILL CAUSE A CRASH");
		}
		if (m_scissorRect.right <= 0)
		{
			Q_RENDER_ERROR("SCISSOR DIMENSIONS HAVE NOT BEEN SET BEFORE RENDERING, THIS WILL CAUSE A CRASH");
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

	void DirectXRendererAPI::SetFinalRenderTextureId(void* textureId)
	{
		m_framebuffer = textureId;
	}



	void* DirectXRendererAPI::GetFinalRenderTextureId()
	{
		return m_framebuffer;
	}

	void DirectXRendererAPI::WaitForGPU()
	{
		for (int i = 0; i < 3; ++i)
		{
			m_fences[i]->Signal();
			m_fences[i]->WaitForFenceEvent();
			m_fences[i]->IncrementFenceValue();
		}
	}

	void DirectXRendererAPI::BlitToTexture(Ref<Texture2D> srcTx, Ref<Texture2D> dstTx)
	{
		DirectXContext.StartBlit(std::static_pointer_cast<DirectXTexture2D>(srcTx)->GetTextureHandle(), std::static_pointer_cast<DirectXTexture2D>(dstTx)->GetTextureHandle(), srcTx->GetWidth(), srcTx->GetHeight());
		DirectXContext.EndBlit();
	}

	void DirectXRendererAPI::BlitToSwapchain(Ref<Texture2D> srcTx)
	{
		DirectXContext.StartBlitToSwapchain(std::static_pointer_cast<DirectXTexture2D>(srcTx)->GetTextureHandle(), m_backBufferRenderTarget[m_bufferIndex].Get(), srcTx->GetWidth(), srcTx->GetHeight());
		DirectXContext.EndBlit();
	}

	void DirectXRendererAPI::SetRootConstant(Ref<DirectXCommandBuffer> commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues)
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

	void DirectXRendererAPI::SetRootConstants(Ref<DirectXCommandBuffer> commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues)
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

	void DirectXRendererAPI::SetConstantBufferView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress)
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

	void DirectXRendererAPI::SetShaderResourceView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress)
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

	void DirectXRendererAPI::SetUnorderedAccessView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress)
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

	void DirectXRendererAPI::SetRootDescriptorTable(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 startGPUAddress)
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

	void DirectXRendererAPI::WaitForPreviousFrame()
	{
		IDXGISwapChain3* swapchain = DirectXContext.GetSwapchain();
		//wait for the previous frame to compile
		m_bufferIndex = swapchain->GetCurrentBackBufferIndex();
		m_fences[m_bufferIndex]->Signal();
		m_fences[m_bufferIndex]->WaitForFenceEvent();
		m_fences[m_bufferIndex]->IncrementFenceValue();
	}

	void DirectXRendererAPI::CreateCPUHeaps()
	{
		//Liam Fix - Load the frameheap (Only do this one, we can deal with safely doing this later)
		m_rtvHeap.Create(DirectXContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3); //unbound texture heap
	}

	void DirectXRendererAPI::CreateRenderTargets()
	{
		IDXGISwapChain3* swapchain = DirectXContext.GetSwapchain();
		m_rtvHeapIndex = 0;
		Q_RENDER_INFO("Creating Swapchain backbuffers");
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
			DirectXContext.GetDevice()->CreateRenderTargetView(m_backBufferRenderTarget[i].Get(), NULL, m_rtvHeap.handleCPU(m_rtvHeapIndex));
			m_rtvHeapIndex++;
		}
	}
}