#pragma once

#include "renderer_api.h"
#include "directx12_frame_heap.h"
#include "directx12_heap.h"
#include "directx12_pipeline.h"
#include "directx12_fence.h"
#include <combaseapi.h>

namespace KNR
{
	class Buffer;
	class Technique;
	class DirectX12CommandBuffer;
	class DirectX12RendererAPI
		: public RendererAPI
	{
	public:
		virtual ~DirectX12RendererAPI();
		virtual void Init() override;
		virtual void SetClearColor(const Vector4f& color) override;
		virtual void Clear() const override;
		virtual void SetViewport(const float x, const float y, const float width, const float height);
		virtual void SetWireframeMode(int i);
		virtual void PopState();
		virtual void BeginRenderSilent();
		virtual void BeginRender();
		virtual void EndRender();

		//Command List required functions
		virtual void SetRootConstant(DirectX12CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues);
		virtual void SetRootConstants(DirectX12CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues);
		virtual void SetConstantBufferView(DirectX12CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetShaderResourceView(DirectX12CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetUnorderedAccessView(DirectX12CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetRootDescriptorTable(DirectX12CommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress) override;
		virtual void DrawIndexedInstanced(DirectX12CommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation);
		virtual void DrawIndirect(DirectX12CommandBuffer* commandList, DirectX12IndirectSignature* DirectX12IndirectSignature, UINT pcommandCount, Buffer* pArgumentBuffer, UINT64 ArgumentBufferOffset, Buffer* pCountBuffer, UINT64 CountBufferOffset);
		virtual void DispatchCompute(DirectX12CommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ) override;

		virtual uint32_t AppendBufferRegion(DirectX12CommandBuffer* commandList, Buffer* dstBuffer, Buffer* srcBuffer) override;

		virtual void BindPipeline(DirectX12CommandBuffer* commandList, Technique* technique) override;

		virtual void BindVertexBuffer(DirectX12CommandBuffer* commandList, Buffer* buffer) override;
		virtual void BindIndexBuffer(DirectX12CommandBuffer* commandList, Buffer* buffer)override;
		virtual void BindUniformBuffer(DirectX12CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)override;
		virtual void BindStructuredBuffer(DirectX12CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)override;

		virtual void SetSwapchainRenderTarget();
		virtual void SetFinalRenderTextureId(void* textureId);
		virtual void* GetFinalRenderTextureId();
		virtual void WaitForGPU();
		virtual void BlitToTexture(Texture2D* srcTx, Texture2D* dstTx);
		virtual void BlitToSwapchain(Texture2D* srcTx);
		virtual void Present();
	private:
		void WaitForPreviousFrame();
		void CreateCPUHeaps();
		void CreateRenderTargets();
		void RecordCommandBuffers();

		//For creating the backbuffer for the swapchain
		Microsoft::WRL::ComPtr<ID3D12Resource> m_backBufferRenderTarget[3];
		DirectX12Fence* m_fences[3];

		D3D12_VIEWPORT m_viewport;							//What we see in the output of the rasterizer
		D3D12_RECT m_scissorRect;							//Area to draw into

		//Frame heap stuff
		DirectX12Heap m_rtvHeap;
		int m_rtvHeapIndex;
		int m_bufferIndex;
		int m_width, m_height;

		bool m_imguiEnabled = true;
		bool m_firstFrame;
		void* m_framebuffer = nullptr;
	};
}