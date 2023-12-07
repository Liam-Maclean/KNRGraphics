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
	class DirectX12RendererAPI final
		: public RendererAPI
	{
	public:
		virtual ~DirectX12RendererAPI();
		virtual void Initialize(const WindowDesc& windowDesc) override;
		virtual void SetClearColor(const float r, const float g, const float b, const float a) override;
		virtual void Clear() const override;
		virtual void SetViewport(const float x, const float y, const float width, const float height);
		virtual void BeginRender();
		virtual void EndRender();

		//Command List required functions
		virtual void SetRootConstant(CommandBuffer* commandList, const uint32_t rootParameterIndex, const uint32_t srcData, const uint32_t destOffsetIn32BitValues);
		virtual void SetRootConstants(CommandBuffer* commandList, const uint32_t rootParameterIndex, const uint32_t numValuesSet, void* srcData, const uint32_t destOffsetIn32BitValues);
		virtual void SetConstantBufferView(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 gpuAddress) override;
		virtual void SetShaderResourceView(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 gpuAddress) override;
		virtual void SetUnorderedAccessView(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 gpuAddress) override;
		virtual void SetRootDescriptorTable(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 startGPUAddress) override;
		virtual void DrawIndexedInstanced(CommandBuffer* commandList, const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t baseVertexLocation, const uint32_t startInstanceLocation);
		virtual void DrawIndirect(CommandBuffer* commandList, DirectX12IndirectSignature* DirectX12IndirectSignature, const UINT pcommandCount, Buffer* pArgumentBuffer, const UINT64 ArgumentBufferOffset, Buffer* pCountBuffer, UINT64 CountBufferOffset);
		virtual void DispatchCompute(CommandBuffer* commandList, const uint32_t dispatchGroupCountX, const uint32_t dispatchGroupCountY, const uint32_t dispatchGroupCountZ) override;

		virtual void BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer) override;
		virtual void BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer)override;
		virtual void BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot)override;
		virtual void BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot)override;

		virtual void SetSwapchainRenderTarget();
		virtual void SetFinalRenderTextureId(void* textureId);
		virtual void* GetFinalRenderTextureId();
		virtual void WaitForGPU();
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