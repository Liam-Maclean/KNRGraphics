#pragma once
#include "window.h"
#include "renderer_api.h"

/// <summary>
/// RenderCommand
/// 
/// Nearly every command fired should be done through this render command interface
/// handles the abstraction behind the curtain
/// </summary>

namespace KNR
{
	class Technique;
	class CommandBuffer;
	class RenderCommand
	{
	public:
		inline static void Initialize(const WindowDesc& windowDesc)
		{
 			sRendererAPI->Initialize(windowDesc);
		}

		inline static void BeginFrame()
		{
			sRendererAPI->BeginFrame();
		}

		inline static void EndFrame()
		{
			sRendererAPI->EndFrame();
		}

		inline static void Present()
		{
			sRendererAPI->Present();
		}

		inline static void ClearRenderTarget(CommandBuffer* commandList, bool clearColor, Color color, bool clearDepth, float depthValue)
		{
			sRendererAPI->ClearRenderTarget(commandList, clearColor, color, clearDepth, depthValue);
		}

		inline static void BindRenderTargets(CommandBuffer* commandList, Texture2D* targets, uint32_t count, Texture2D* depthTarget)
		{
			sRendererAPI->BindRenderTargets(commandList, targets, count, depthTarget);
		}

		inline static void SetViewport(CommandBuffer* commandList, const float x, const float y, const float width, const float height)
		{
			sRendererAPI->SetViewport(commandList, x, y, width, height);
		}

		inline static void SetScissor(CommandBuffer* commandList, const float left, const float top, const float right, const float bottom)
		{
			sRendererAPI->SetScissor(commandList, left, top, right, bottom);
		}

		inline static void WaitForGPU()
		{
			sRendererAPI->WaitForGPU();
		}

		inline static void BeginRecordingCommands(CommandBuffer* commandList)
		{
			sRendererAPI->BeginRecordingCommands(commandList);
		}

		inline static void SubmitCommandBufferImmediate(CommandBuffer* commandList)
		{
			sRendererAPI->SubmitCommandBufferImmediate(commandList);
		}

		inline static void SubmitCommandBuffer(CommandBuffer* commandList)
		{
			sRendererAPI->SubmitCommandBuffer(commandList);
		}

		inline static void WaitOnCommandBuffer(CommandBuffer* commandList)
		{
			sRendererAPI->WaitOnCommandList(commandList);
		}

		inline static void BindPipeline(CommandBuffer* commandList, Pipeline* pipeline)
		{
			sRendererAPI->BindPipeline(commandList, pipeline);
		}

		inline static void Draw(CommandBuffer* commandList)
		{
			sRendererAPI->Draw(commandList);
		}

		inline static void DrawIndexed(CommandBuffer* commandList)
		{
			sRendererAPI->DrawIndexed(commandList);
		}

		inline static void DispatchCompute(CommandBuffer* commandList, const uint32_t dispatchThreadCountX, const uint32_t dispatchThreadCountY, const uint32_t dispatchThreadCountZ)
		{
			sRendererAPI->DispatchCompute(commandList, dispatchThreadCountX, dispatchThreadCountY, dispatchThreadCountZ);
		}

		inline static void DrawIndirectInstanced(CommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t baseIndex, uint32_t baseVertex, uint32_t baseInstance)
		{
			sRendererAPI->DrawIndexedInstanced(commandList, indexCount, instanceCount, baseIndex, baseVertex, baseInstance);
		}

		inline static void DrawIndirect(CommandBuffer* commandList, DirectX12IndirectSignature* DirectX12IndirectSignature, const uint32_t commandCount, Buffer* commandArgBuffer, const uint32_t argOffset, Buffer* countBuffer, const uint32_t countBufferOffset)
		{
			sRendererAPI->DrawIndirect(commandList, DirectX12IndirectSignature, commandCount, commandArgBuffer, argOffset, countBuffer, countBufferOffset);
		}

		inline static void ImmediateDrawBegin(TopologyIndexMethod topologyMethod)
		{
			sRendererAPI->ImmediateDrawBegin(topologyMethod);
		}

		inline static void ImmediateVertex(float posX, float posY, float posZ, float u, float v)
		{
			sRendererAPI->ImmediateVertex(posX, posY, posZ, u, v);
		}

		inline static void ImmediateDrawEnd(CommandBuffer* commandList)
		{
			sRendererAPI->ImmediateDrawEnd(commandList);
		}


		inline static void BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer)
		{
			sRendererAPI->BindVertexBuffer(commandList, buffer);
		}

		inline static void BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer)
		{
			sRendererAPI->BindIndexBuffer(commandList,buffer);
		}

		inline static void BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot)
		{
			sRendererAPI->BindUniformBuffer(commandList, buffer, bindslot);
		}


		inline static void BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot)
		{
			sRendererAPI->BindStructuredBuffer(commandList, buffer, bindslot);
		}





	private:
		static RendererAPI* sRendererAPI;
	};
}