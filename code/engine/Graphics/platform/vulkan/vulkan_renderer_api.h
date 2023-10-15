#pragma once

#include "renderer_api.h"
#include "vulkan_pipeline.h"
#include "vulkan_fence.h"
#include <combaseapi.h>

namespace KNR
{
	class Buffer;
	class CommandBuffer;
	class Technique;
	class VulkanRendererAPI
		: public RendererAPI
	{
	public:
		virtual ~VulkanRendererAPI();

		//Constructor
		virtual void Initialize() override;

		//Swapchain
		virtual void Present() override;
		virtual void BeginRender() override;
		virtual void EndRender() override;
		virtual void BeginRenderSilent() override;
		virtual void SetSwapchainRenderTarget() override;

		//Viewport calls
		virtual void Clear() const override;
		virtual void SetClearColor(float r, float g, float b, float a) override;
		virtual void SetViewport(const float x, const float y, const float w, const float h) override;
		virtual void SetWireframeMode(int i) override;

		//CommandBuffer calls
		virtual void WaitForGPU() override;

		//Render Calls
		virtual void DispatchCompute(CommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ) override;
		virtual void DrawIndirect(CommandBuffer* commandList, IndirectSignature* indirectSignature, UINT pcommandCount, Buffer* commandBuffer, UINT64 ArgumentBufferOffset, Buffer* pCountBuffer, UINT64 CountBufferOffset) override;
		virtual void DrawIndexedInstanced(CommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation) override;
		virtual void BlitToTexture(Texture2D* srcTx, Texture2D* dstTx) override;
		virtual void BlitToSwapchain(Texture2D* srcTx) override;

		//Binds
		virtual void BindPipeline(CommandBuffer* commandList, Technique* technique) override;
		virtual void BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer) override;
		virtual void BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer) override;
		virtual void BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot) override;
		virtual void BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot) override;

		virtual void SetRootConstant(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) override;
		virtual void SetRootConstants(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues) override;
		virtual void SetConstantBufferView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetShaderResourceView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetUnorderedAccessView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) override;
		virtual void SetRootDescriptorTable(CommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress) override;

		//Util
		virtual uint32_t AppendBufferRegion(CommandBuffer* commandList, Buffer* dstBuffer, Buffer* srcBuffer) override;
		virtual void SetFinalRenderTextureId(void* textureId) override;
		virtual void* GetFinalRenderTextureId() override;

	private:
		void WaitForPreviousFrame();
		void CreateCPUHeaps();
		void CreateRenderTargets();
		void RecordCommandBuffers();

		//Frame heap stuff
		int m_width, m_height;
		bool m_firstFrame;
		void* m_framebuffer = nullptr;
	};
}