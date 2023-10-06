#pragma once
#pragma once

#include "graphics/renderer_api.h"
#include "graphics/platform/directx/directx_frame_heap.h"
#include "graphics/platform/directx/directx_heap.h"
#include "graphics/platform/directx/directx_pipeline.h"
#include "graphics/platform/directx/directx_fence.h"
#include <combaseapi.h>
#include "core/core.h"

namespace QRender
{
	class Buffer;
	class Technique;
	class DirectXCommandBuffer;
	class DirectXRendererAPI
		: public RendererAPI
	{
	public:
		virtual ~DirectXRendererAPI();
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
		virtual void SetRootConstant(Ref<DirectXCommandBuffer> commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues);
		virtual void SetRootConstants(Ref<DirectXCommandBuffer> commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues);
		virtual void SetConstantBufferView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetShaderResourceView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetUnorderedAccessView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetRootDescriptorTable(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 startGPUAddress) override;
		virtual void DrawIndexedInstanced(Ref<DirectXCommandBuffer> commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation);
		virtual void DrawIndirect(Ref<DirectXCommandBuffer> commandList, Ref<IndirectSignature> indirectSignature, UINT pcommandCount, Ref<Buffer> pArgumentBuffer, UINT64 ArgumentBufferOffset, Ref<Buffer> pCountBuffer, UINT64 CountBufferOffset);
		virtual void DispatchCompute(Ref<DirectXCommandBuffer> commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ) override;
		virtual void DrawIndexed(Ref<DirectXCommandBuffer> commandList, const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;

		virtual uint32_t AppendBufferRegion(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> dstBuffer, Ref<QRender::Buffer> srcBuffer) override;

		virtual void BindPipeline(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Technique> technique) override;

		virtual void BindVertexBuffer(Ref<DirectXCommandBuffer> commandList, Ref<Buffer> buffer) override;
		virtual void BindIndexBuffer(Ref<DirectXCommandBuffer> commandList, Ref<Buffer> buffer)override;
		virtual void BindUniformBuffer(Ref<DirectXCommandBuffer> commandList, Ref<Buffer> buffer, uint32_t bindslot)override;
		virtual void BindStructuredBuffer(Ref<DirectXCommandBuffer> commandList, Ref<Buffer> buffer, uint32_t bindslot)override;

		virtual void SetSwapchainRenderTarget();
		virtual void SetFinalRenderTextureId(void* textureId);
		virtual void* GetFinalRenderTextureId();
		virtual void WaitForGPU();
		virtual void BlitToTexture(Ref<Texture2D> srcTx, Ref<Texture2D> dstTx);
		virtual void BlitToSwapchain(Ref<Texture2D> srcTx);
		virtual void Present();
	private:
		void WaitForPreviousFrame();
		void CreateCPUHeaps();
		void CreateRenderTargets();
		void RecordCommandBuffers();

		//For creating the backbuffer for the swapchain
		Microsoft::WRL::ComPtr<ID3D12Resource> m_backBufferRenderTarget[3];
		DirectXFence* m_fences[3];

		D3D12_VIEWPORT m_viewport;							//What we see in the output of the rasterizer
		D3D12_RECT m_scissorRect;							//Area to draw into

		//Frame heap stuff
		DirectXHeap m_rtvHeap;
		int m_rtvHeapIndex;
		int m_bufferIndex;
		int m_width, m_height;

		bool m_imguiEnabled = true;
		bool m_firstFrame;
		void* m_framebuffer = nullptr;
	};
}