#pragma once
#include <basetsd.h>
#include <minwindef.h>
#include "window.h"
namespace KNR
{
	class VertexArray;
	class Texture2D;
	class DirectX12IndirectSignature;
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
		virtual void Initialize(const WindowDesc& windowDesc) = 0;

		//Swapchain
		virtual void Present() = 0;
		virtual void BeginRender() = 0;
		virtual void EndRender() = 0;
		virtual void SetSwapchainRenderTarget() = 0;

		//Viewport calls
		virtual void Clear() const = 0;
		virtual void SetClearColor(const float r, const float g, const float b, const float a) = 0;
		virtual void SetViewport(const float x, const float y, const float w, const float h) = 0;
		virtual void SetScissor(const float left, const float top, const float right, const float bottom) = 0;
		virtual void SetWireframeMode(const int i) = 0;

		//CommandBuffer calls
		virtual void WaitForGPU() = 0;
		virtual void BeginCommandList() = 0;
		virtual void SubmitCommandBufferImmediate(CommandBuffer* commandList) = 0;
		virtual void SubmitCommandBuffer(CommandBuffer* commandList) = 0;
		virtual void WaitOnCommandList(CommandBuffer* commandList) = 0;

		//Immediate rendering calls 
		virtual void BeginDrawImmediate(TopologyIndexMethod indexMethod) = 0;
		virtual void DrawImmedateVertex(float x, float y, float z, float w) = 0;
		virtual void EndDrawImmediate() = 0;

		//Render Calls
		virtual void Draw(CommandBuffer* commandList) = 0;
		virtual void DrawIndexed(CommandBuffer* commandList) = 0;
		virtual void DispatchCompute(CommandBuffer* commandList, const uint32_t dispatchGroupCountX, const uint32_t dispatchGroupCountY, const uint32_t dispatchGroupCountZ) = 0;
		virtual void DrawIndexedInstanced(CommandBuffer* commandList, const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t baseVertexLocation, const uint32_t startInstanceLocation) = 0;
		virtual void DrawIndirect(CommandBuffer* commandList, DirectX12IndirectSignature* DirectX12IndirectSignature, const UINT pcommandCount, Buffer* commandBuffer, const UINT64 ArgumentBufferOffset, Buffer* pCountBuffer, const UINT64 CountBufferOffset) = 0;

		//Binds
		virtual void BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer) = 0;
		virtual void BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer) = 0;
		virtual void BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot) = 0;
		virtual void BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot) = 0;
		
		virtual void SetRootConstant(CommandBuffer* commandList, const uint32_t rootParameterIndex, const uint32_t srcData, const uint32_t destOffsetIn32BitValues) = 0;
		virtual void SetRootConstants(CommandBuffer* commandList, const uint32_t rootParameterIndex, const uint32_t numValuesSet, void* srcData, const uint32_t destOffsetIn32BitValues) = 0;
		virtual void SetConstantBufferView(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 gpuAddress) = 0;
		virtual void SetShaderResourceView(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 gpuAddress) = 0;
		virtual void SetUnorderedAccessView(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 gpuAddress) = 0;
		virtual void SetRootDescriptorTable(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 startGPUAddress) = 0;

		//Util
		virtual uint32_t AppendBufferRegion(CommandBuffer* commandList, Buffer* dstBuffer, Buffer* srcBuffer) = 0;
		virtual void SetFinalRenderTextureId(void* textureId) = 0;
		virtual void* GetFinalRenderTextureId() = 0;

		inline static API GetAPI() { return s_target_api; }
	private:
		static API s_target_api;
	};
}
