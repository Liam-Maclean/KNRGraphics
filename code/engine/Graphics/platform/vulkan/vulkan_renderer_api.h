#pragma once

#include "renderer_api.h"
#include "vulkan_pipeline.h"
#include "vulkan_fence.h"
#include <combaseapi.h>

namespace KNR
{
	class Buffer;
	class Technique;
	class VulkanRendererAPI
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
		virtual void SetRootConstant(DirectXCommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues);
		virtual void SetRootConstants(DirectXCommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues);
		virtual void SetConstantBufferView(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetShaderResourceView(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetUnorderedAccessView(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetRootDescriptorTable(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress) override;
		virtual void DrawIndexedInstanced(DirectXCommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation);
		virtual void DrawIndirect(DirectXCommandBuffer* commandList, IndirectSignature* indirectSignature, UINT pcommandCount, Buffer* pArgumentBuffer, UINT64 ArgumentBufferOffset, Buffer* pCountBuffer, UINT64 CountBufferOffset);
		virtual void DispatchCompute(DirectXCommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ) override;

		virtual uint32_t AppendBufferRegion(DirectXCommandBuffer* commandList, Buffer* dstBuffer, Buffer* srcBuffer) override;

		virtual void BindPipeline(DirectXCommandBuffer* commandList, Technique* technique) override;

		virtual void BindVertexBuffer(DirectXCommandBuffer* commandList, Buffer* buffer) override;
		virtual void BindIndexBuffer(DirectXCommandBuffer* commandList, Buffer* buffer)override;
		virtual void BindUniformBuffer(DirectXCommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)override;
		virtual void BindStructuredBuffer(DirectXCommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)override;

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

		//Frame heap stuff
		int m_rtvHeapIndex;
		int m_bufferIndex;
		int m_width, m_height;

		bool m_imguiEnabled = true;
		bool m_firstFrame;
		void* m_framebuffer = nullptr;
	};
}