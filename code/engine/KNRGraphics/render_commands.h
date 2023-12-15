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

		inline static void ClearRenderTarget(CommandBuffer* commandList, bool clearColor, Color color, bool clearDepth, float depthValue)
		{
			sRendererAPI->ClearRenderTarget(commandList, clearColor, color, clearDepth, depthValue);
		}

		inline static void IndirectDraw(CommandBuffer* commandList, DirectX12IndirectSignature* DirectX12IndirectSignature, const uint32_t commandCount, Buffer* commandArgBuffer, const uint32_t argOffset, Buffer* countBuffer, const uint32_t countBufferOffset)
		{
			sRendererAPI->DrawIndirect(commandList, DirectX12IndirectSignature, commandCount, commandArgBuffer, argOffset, countBuffer, countBufferOffset);
		}

		inline static void DispatchCompute(CommandBuffer* commandList, const uint32_t dispatchThreadCountX, const uint32_t dispatchThreadCountY, const uint32_t dispatchThreadCountZ)
		{
			sRendererAPI->DispatchCompute(commandList, dispatchThreadCountX, dispatchThreadCountY, dispatchThreadCountZ);
		}

		inline static void SetViewport(CommandBuffer* commandList, const float x, const float y, const float width, const float height)
		{
			sRendererAPI->SetViewport(commandList, x, y, width, height);
		}

		inline static void SetScissor(CommandBuffer* commandList, const float left, const float top, const float right, const float bottom)
		{
			sRendererAPI->SetScissor(commandList, left, top, right, bottom);
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

		inline static void BindPipeline(CommandBuffer* commandList, Pipeline* pipeline)
		{
			sRendererAPI->BindPipeline(commandList, pipeline);
		}

		inline static void BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, const uint32_t bindslot)
		{
			sRendererAPI->BindStructuredBuffer(commandList, buffer, bindslot);
		}

		inline static void WaitForGPU()
		{
			sRendererAPI->WaitForGPU();
		}

		inline static void BeginRender()
		{
			sRendererAPI->BeginFrame();
		}

		inline static void EndRender()
		{
			sRendererAPI->EndFrame();
		}

		inline static void Present()
		{
			sRendererAPI->Present();
		}

	private:
		static RendererAPI* sRendererAPI;
	};
}