#pragma once
#include "window.h"
#include "renderer_api.h"

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

		inline static void SetClearColor(const float r, const float g, const float b, const float a)
		{
			sRendererAPI->SetClearColor(r,g,b,a);
		}

		inline static void ClearRenderTarget(CommandBuffer* commandList)
		{
			sRendererAPI->Clear();
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
			sRendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetScissor(CommandBuffer* commandList, const float left, const float top, const float right, const float bottom)
		{
			sRendererAPI->SetScissor(left, top, right, bottom);
		}

		inline static void SetWireframeMode(const int i)
		{
			sRendererAPI->SetWireframeMode(i);
		}

		inline static void SetSwapchainRenderTarget()
		{
			sRendererAPI->SetSwapchainRenderTarget();
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

		inline static void SetRootConstant(CommandBuffer* commandList, const uint32_t rootParameterIndex, const uint32_t srcData, const uint32_t destOffsetIn32BitValues)
		{
			sRendererAPI->SetRootConstant(commandList, rootParameterIndex, srcData, destOffsetIn32BitValues);
		}

		inline static void SetRootConstants(CommandBuffer* commandList, const uint32_t rootParameterIndex, const uint32_t numValuesSet, void* data, const uint32_t destOffsetIn32BitValues)
		{
			sRendererAPI->SetRootConstants(commandList, rootParameterIndex, numValuesSet, data, destOffsetIn32BitValues);
		}

		inline static void SetConstantBufferView(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 gpuAddress)
		{
			sRendererAPI->SetConstantBufferView(commandList, bindSlot, gpuAddress);
		}

		inline static void SetShaderResourceView(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 gpuAddress)
		{
			sRendererAPI->SetShaderResourceView(commandList, bindSlot, gpuAddress);
		}

		inline static void SetUnorderedAccessView(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 gpuAddress)
		{
			sRendererAPI->SetUnorderedAccessView(commandList, bindSlot, gpuAddress);
		}

		inline static void SetRootDescriptorTable(CommandBuffer* commandList, const uint32_t bindSlot, const UINT64 startGPUAddress)
		{
			sRendererAPI->SetRootDescriptorTable(commandList, bindSlot, startGPUAddress);
		}

		inline static void WaitForGPU()
		{
			sRendererAPI->WaitForGPU();
		}

		inline static void BeginRender()
		{
			sRendererAPI->BeginRender();
		}

		inline static void EndRender()
		{
			sRendererAPI->EndRender();
		}

		inline static void Present()
		{
			sRendererAPI->Present();
		}

	private:
		static RendererAPI* sRendererAPI;
	};
}