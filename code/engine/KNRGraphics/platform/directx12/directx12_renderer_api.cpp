#include "directx12_buffer.h"
#include "directx12_graphics_context.h"
#include "directx12_renderer_api.h"
#include "directx12_heap.h"
#include "directx12_texture.h"
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
		m_width = windowDesc.width;
		m_height = windowDesc.height;

		m_rtvHeapIndex = 0;
		for (int i = 0; i < 3; ++i)
		{
			m_fences[i] = new DirectX12Fence(false);
		}
		CreateCPUHeaps();
		CreateRenderTargets();
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

	}

	void DirectX12RendererAPI::ImmediateVertex(float posX, float posY, float posZ, float u, float v)
	{

	}

	void DirectX12RendererAPI::ImmediateDrawEnd(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandList = static_cast<DirectX12CommandBuffer*>(commandList);
	}

	void DirectX12RendererAPI::SetViewport(CommandBuffer* commandList, const float x, const float y, const float width, const float height)
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

	void DirectX12RendererAPI::SetScissor(CommandBuffer* commandList, const float left, const float top, const float right, const float bottom)
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
	}

	void DirectX12RendererAPI::SubmitCommandBuffer(CommandBuffer* commandList)
	{
	}

	void DirectX12RendererAPI::WaitOnCommandList(CommandBuffer* commandList)
	{
	}

	void DirectX12RendererAPI::BindPipeline(CommandBuffer* commandList, Pipeline* pipeline)
	{
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