#include "directx12_buffer.h"
#include "directx12_graphics_context.h"
#include "directx12_renderer_api.h"
#include "directx12_heap.h"
#include "directx12_texture.h"
#include "directx12_indirect_signature.h"
#include "directx12_util.h"
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
		m_width = windowDesc.width;
		m_height = windowDesc.height;

		DirectX12Context.Init(windowDesc);

		for (int i = 0; i < 3; ++i)
		{
			m_fences[i] = new DirectX12Fence(false);
		}
		CreateSwapchainBackbuffer();
	}

	void DirectX12RendererAPI::ClearRenderTarget(CommandBuffer* commandList, bool clearColor, Color color, bool clearDepth, float depthValue) const
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
		//directXCommandList->Get()->ClearRenderTargetView()
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
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
	}

	void DirectX12RendererAPI::BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{

	}

	void DirectX12RendererAPI::Draw(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
	}

	void DirectX12RendererAPI::DrawIndexed(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
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
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

	}

	void DirectX12RendererAPI::ImmediateDrawBegin(TopologyIndexMethod indexMethod)
	{
		m_immediateIndexMethod = indexMethod;
	}

	void DirectX12RendererAPI::ImmediateVertex(float posX, float posY, float posZ, float u, float v)
	{
		ImVert newImVert = {};
		newImVert.position = XMFLOAT4(posX, posY, posZ, 1.0f);
		newImVert.uv = XMFLOAT2(u, v);
		m_immediateNewVertices.push_back(newImVert);
	}

	void DirectX12RendererAPI::ImmediateDrawEnd(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		Draw(commandList);
	}

	void DirectX12RendererAPI::SetViewport(CommandBuffer* commandList, const float x, const float y, const float width, const float height)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

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


		directXCommandList->Get()->RSSetViewports(1, &m_viewport);
		
	}

	void DirectX12RendererAPI::SetScissor(CommandBuffer* commandList, const float left, const float top, const float right, const float bottom)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);

		//This should never be hit
		assert(right > 0);
		assert(bottom > 0);

		m_scissorRect.left = left;
		m_scissorRect.top = top;
		m_scissorRect.right = right;
		m_scissorRect.bottom = bottom;

		directXCommandList->Get()->RSSetScissorRects(1, &m_scissorRect);
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
	void DirectX12RendererAPI::BeginFrame()
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
	void DirectX12RendererAPI::EndFrame()
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

	void DirectX12RendererAPI::ResizeWindow(const float width, const float height)
	{
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

		CreateSwapchainBackbuffer();
	}

	void DirectX12RendererAPI::Present()
	{
		IDXGISwapChain3* m_swapChain = DirectX12Context.GetSwapchain();
		HRESULT hr = m_swapChain->Present(0, 0);
		if (FAILED(hr))
		{
			assert(0);
		}
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

	void DirectX12RendererAPI::SubmitCommandBufferImmediate(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
		directXCommandList->SubmitWorkImmediate();
	}

	void DirectX12RendererAPI::SubmitCommandBuffer(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
		directXCommandList->Close();
		directXCommandList->Submit(DirectX12Context.GetCommandQueue());
	}

	void DirectX12RendererAPI::WaitOnCommandList(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
		directXCommandList->Wait();
	}

	void DirectX12RendererAPI::BeginRecordingCommands(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
		directXCommandList->Reset();
	}

	void DirectX12RendererAPI::BindPipeline(CommandBuffer* commandList, Pipeline* pipeline)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
		DirectX12Pipeline* directXPipeline = static_cast<DirectX12Pipeline*>(pipeline);

		//Binds the pipeline, binds the data
		directXCommandList->Get()->SetPipelineState(directXPipeline->GetPipeline());
		directXCommandList->Get()->SetGraphicsRootSignature(directXPipeline->GetRootSignature());
		directXCommandList->Get()->SetGraphicsRootDescriptorTable((int)KNRRootSignatureBindSlot::Texture1DSlot, DirectX12Context.GetReservedHeap(ReservedHeapRegion::BindlessTexture1D)->hGPUHeapStart);
		directXCommandList->Get()->SetGraphicsRootDescriptorTable((int)KNRRootSignatureBindSlot::Texture2DSlot, DirectX12Context.GetReservedHeap(ReservedHeapRegion::BindlessTexture2D)->hGPUHeapStart);
		directXCommandList->Get()->SetGraphicsRootDescriptorTable((int)KNRRootSignatureBindSlot::Texture2DArraySlot, DirectX12Context.GetReservedHeap(ReservedHeapRegion::BindlessTexture2DArray)->hGPUHeapStart);
		directXCommandList->Get()->SetGraphicsRootDescriptorTable((int)KNRRootSignatureBindSlot::Texture3DSlot, DirectX12Context.GetReservedHeap(ReservedHeapRegion::BindlessTexture3D)->hGPUHeapStart);
		directXCommandList->Get()->SetGraphicsRootDescriptorTable((int)KNRRootSignatureBindSlot::TextureCubemapSlot, DirectX12Context.GetReservedHeap(ReservedHeapRegion::BindlessCubemap)->hGPUHeapStart);
		directXCommandList->Get()->SetGraphicsRootDescriptorTable((int)KNRRootSignatureBindSlot::TextureCubemapArraySlot, DirectX12Context.GetReservedHeap(ReservedHeapRegion::BindlessCubemapArray)->hGPUHeapStart);
		directXCommandList->Get()->SetGraphicsRootDescriptorTable((int)KNRRootSignatureBindSlot::ConstantSlot, DirectX12Context.GetReservedHeap(ReservedHeapRegion::BindlessConstant)->hGPUHeapStart);
	}

	void DirectX12RendererAPI::BindRenderTargets(CommandBuffer* commandList, Texture2D* targets, uint32_t count, Texture2D* depthTarget)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetHandles;
		for (size_t i = 0; i < count; ++i)
		{
			DirectX12Texture2D* directXTexture = static_cast<DirectX12Texture2D*>(&targets[i]);
			renderTargetHandles.push_back(static_cast<D3D12_CPU_DESCRIPTOR_HANDLE>(directXTexture->GetRTBlock().cpuHandle));
		}

		D3D12_CPU_DESCRIPTOR_HANDLE depthTargetHandle = {};
		//If we have a depth target, bind that as well
		if (depthTarget)
		{
			DirectX12Texture2D* directXDepthTexture = static_cast<DirectX12Texture2D*>(depthTarget);
			depthTargetHandle = directXDepthTexture->GetRTBlock().cpuHandle;
			directXCommandList->Get()->OMSetRenderTargets(renderTargetHandles.size(), renderTargetHandles.data(), false, &depthTargetHandle);
		}
		else //Otherwise, don't bind it 
		{
			directXCommandList->Get()->OMSetRenderTargets(renderTargetHandles.size(), renderTargetHandles.data(), false, nullptr);
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

	void DirectX12RendererAPI::CreateSwapchainBackbuffer()
	{
		IDXGISwapChain3* swapchain = DirectX12Context.GetSwapchain();
		
		//Loop for each frame in flight
		for (size_t i = 0; i < 3; ++i)
		{
			HRESULT result;
			//Get a pointer to the first back buffer from the swapchain
			result = swapchain->GetBuffer(i, IID_PPV_ARGS(&m_backBufferRenderTarget[i]));
			if (FAILED(result))
			{
				KNT_ERROR("Failed to get backbuffer from swapchain")
				assert(0);
			}
			m_backBufferRenderTarget[i]->SetName(L"Swapchain Back Buffer");
			m_backbufferHandleBlocks[i] = DirectX12Context.ReserveDescriptorHandle(ReservedHeapRegion::RenderTarget);
			DirectX12Context.CreateBackbufferRTV(m_backBufferRenderTarget[i].Get(), m_backbufferHandleBlocks[i]);
		}
	}
}