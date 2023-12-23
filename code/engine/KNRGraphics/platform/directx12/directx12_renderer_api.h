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
	class DirectX12CommandBuffer;
	class DirectX12RendererAPI final
		: public RendererAPI
	{
	public:
		//Constructor/destructor initialization
		virtual ~DirectX12RendererAPI();
		virtual void Initialize(const WindowDesc& windowDesc) override;

		//Swapchain
		virtual void Present();
		virtual void BeginFrame();
		virtual void EndFrame();
	
		//Viewport
		virtual void ClearRenderTarget(CommandBuffer* commandList, bool clearColor, Color color, bool clearDepth, float depthValue) const override;
		virtual void SetViewport(CommandBuffer* commandList, const float x, const float y, const float width, const float height) override;
		virtual void SetScissor(CommandBuffer* commandList, const float left, const float top, const float right, const float bottom) override;

		//Commandbuffer calls
		virtual void WaitForGPU() override;
		virtual void SubmitCommandBufferImmediate(CommandBuffer* commandList) override;
		virtual void SubmitCommandBuffer(CommandBuffer* commandList) override;
		virtual void WaitOnCommandList(CommandBuffer* commandList) override;

		//Render pipeline
		virtual void BindPipeline(CommandBuffer* commandList, Pipeline* pipeline) override;

		//Render Calls
		virtual void Draw(CommandBuffer* commandList) override;
		virtual void DrawIndexed(CommandBuffer* commandList) override;
		virtual void DrawIndexedInstanced(CommandBuffer* commandList, const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t baseVertexLocation, const uint32_t startInstanceLocation);
		virtual void DrawIndirect(CommandBuffer* commandList, DirectX12IndirectSignature* DirectX12IndirectSignature, const UINT pcommandCount, Buffer* pArgumentBuffer, const UINT64 ArgumentBufferOffset, Buffer* pCountBuffer, UINT64 CountBufferOffset);
		virtual void DispatchCompute(CommandBuffer* commandList, const uint32_t dispatchGroupCountX, const uint32_t dispatchGroupCountY, const uint32_t dispatchGroupCountZ) override;

		//Immediate rendering
		virtual void ImmediateDrawBegin(TopologyIndexMethod indexMethod) override;
		virtual void ImmediateVertex(float posX, float posY, float posZ, float u, float v) override;
		virtual void ImmediateDrawEnd(CommandBuffer* commandList) override;

		//Buffer binds
		virtual void BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer) override;
		virtual void BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer)override;
		virtual void BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot)override;
		virtual void BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot)override;

	private:
		void WaitForPreviousFrame();
		void CreateRenderTargets();
		void RecordCommandBuffers();

		//For creating the backbuffer for the swapchain
		Microsoft::WRL::ComPtr<ID3D12Resource> m_backBufferRenderTarget[3];
		DirectX12Fence* m_fences[3];
		DirectX12DescriptorHandleBlock m_backbufferHandleBlocks[3];

		D3D12_VIEWPORT m_viewport;							//What we see in the output of the rasterizer
		D3D12_RECT m_scissorRect;							//Area to draw into

		//Frame heap stuff
		int m_bufferIndex;
		int m_width, m_height;
	};
}