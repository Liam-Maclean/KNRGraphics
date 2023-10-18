#pragma once
#include <basetsd.h>
#include <minwindef.h>
namespace KNR
{
	class VertexArray;
	class Texture2D;
	class IndirectSignature;
	class Buffer;
	class Technique;
	class UniformBuffer;
	class StructuredBuffer;
	class CommandBuffer;
	class RendererAPI
	{
	public:

		enum class API
		{
			None = 0,
			OpenGL = 1,
			Vulkan = 2,
			DirectX11 = 3,
			DirectX12 = 3,
		};

	public:

		//Constructor
		virtual ~RendererAPI() = 0 {};
		virtual void Initialize() = 0;

		//Swapchain
		virtual void Present() = 0;
		virtual void BeginRender() = 0;
		virtual void EndRender() = 0;
		virtual void BeginRenderSilent() = 0;
		virtual void SetSwapchainRenderTarget() = 0;

		//Viewport calls
		virtual void Clear() const = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual void SetViewport(const float x, const float y, const float w, const float h) = 0;
		virtual void SetScissor(const float left, const float top, const float right, const float bottom) = 0;
		virtual void SetWireframeMode(int i) = 0;

		//CommandBuffer calls
		virtual void WaitForGPU() = 0;
		virtual void BeginCommandList() = 0;
		virtual void SubmitCommandBufferImmediate(CommandBuffer* commandList) = 0;
		virtual void SubmitCommandBuffer(CommandBuffer* commandList) = 0;
		virtual void WaitOnCommandList(CommandBuffer* commandList) = 0;


		//Render Calls
		virtual void DispatchCompute(CommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ) = 0;
		virtual void DrawIndexedInstanced(CommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation) = 0;
		virtual void DrawIndirect(CommandBuffer* commandList, IndirectSignature* indirectSignature, UINT pcommandCount, Buffer* commandBuffer, UINT64 ArgumentBufferOffset, Buffer* pCountBuffer, UINT64 CountBufferOffset) = 0;
		virtual void BlitToTexture(Texture2D* srcTx, Texture2D* dstTx) = 0;
		virtual void BlitToSwapchain(Texture2D* srcTx) = 0;

		//Binds
		virtual void BindPipeline(CommandBuffer* commandList, Technique* technique) = 0;
		virtual void BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer) = 0;
		virtual void BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer) = 0;
		virtual void BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot) = 0;
		virtual void BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot) = 0;
		
		virtual void SetRootConstant(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) = 0;
		virtual void SetRootConstants(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues) = 0;
		virtual void SetConstantBufferView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) = 0;
		virtual void SetShaderResourceView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) = 0;
		virtual void SetUnorderedAccessView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) = 0;
		virtual void SetRootDescriptorTable(CommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress) = 0;

		//Util
		virtual uint32_t AppendBufferRegion(CommandBuffer* commandList, Buffer* dstBuffer, Buffer* srcBuffer) = 0;
		virtual void SetFinalRenderTextureId(void* textureId) = 0;
		virtual void* GetFinalRenderTextureId() = 0;

		inline static API GetAPI() { return s_target_api; }
	private:
		static API s_target_api;
	};
}