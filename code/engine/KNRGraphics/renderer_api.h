#pragma once
#include <basetsd.h>
#include <minwindef.h>
#include "window.h"
#include "render_types.h"

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
	class Pipeline;

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
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void ResizeWindow(const float width, const float height) = 0;

		//Viewport calls
		virtual void ClearRenderTarget(CommandBuffer* commandList, bool clearColor, Color color, bool clearDepth, float depthValue) const = 0;
		virtual void SetViewport(CommandBuffer* commandList, const float x, const float y, const float w, const float h) = 0;
		virtual void SetScissor(CommandBuffer* commandList, const float left, const float top, const float right, const float bottom) = 0;

		//CommandBuffer calls
		virtual void WaitForGPU() = 0;
		virtual void SubmitCommandBufferImmediate(CommandBuffer* commandList) = 0;
		virtual void SubmitCommandBuffer(CommandBuffer* commandList) = 0;
		virtual void WaitOnCommandList(CommandBuffer* commandList) = 0;
		virtual void BeginRecordingCommands(CommandBuffer* commandList) = 0;

		//Render pipeline
		virtual void BindPipeline(CommandBuffer* commandList, Pipeline* pipeline) = 0;
		virtual void BindRenderTargets(CommandBuffer* commandList, Texture2D* targets, uint32_t count, Texture2D* depthTarget) = 0;

		//Render Calls
		virtual void Draw(CommandBuffer* commandList) = 0;
		virtual void DrawIndexed(CommandBuffer* commandList) = 0;
		virtual void DispatchCompute(CommandBuffer* commandList, const uint32_t dispatchGroupCountX, const uint32_t dispatchGroupCountY, const uint32_t dispatchGroupCountZ) = 0;
		virtual void DrawIndexedInstanced(CommandBuffer* commandList, const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const uint32_t baseVertexLocation, const uint32_t startInstanceLocation) = 0;
		virtual void DrawIndirect(CommandBuffer* commandList, DirectX12IndirectSignature* DirectX12IndirectSignature, const UINT pcommandCount, Buffer* commandBuffer, const UINT64 ArgumentBufferOffset, Buffer* pCountBuffer, const UINT64 CountBufferOffset) = 0;

		//Immediate rendering
		virtual void ImmediateDrawBegin(TopologyIndexMethod indexMethod) = 0;
		virtual void ImmediateVertex(float posX, float posY, float posZ, float u, float v) = 0;
		virtual void ImmediateDrawEnd(CommandBuffer* commandList) = 0;

		//Binds
		virtual void BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer) = 0;
		virtual void BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer) = 0;
		virtual void BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot) = 0;
		virtual void BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot) = 0;

		inline static API GetAPI() { return s_target_api; }
	private:
		static API s_target_api;
	};
}
