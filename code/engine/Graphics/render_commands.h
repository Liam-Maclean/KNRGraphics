#pragma once

#include "renderer_api.h"

namespace KNR
{
	class Technique;
	class DirectXCommandBuffer;
	class RenderCommand
	{
	public:
		inline static void Init()
		{
 			sRendererAPI->Init();
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			sRendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			sRendererAPI->Clear();
		}

		inline static void IndirectDraw(DirectXCommandBuffer* commandList, IndirectSignature* indirectSignature, uint32_t commandCount, Buffer* commandArgBuffer, uint32_t argOffset, Buffer* countBuffer, uint32_t countBufferOffset)
		{
			sRendererAPI->DrawIndirect(commandList, indirectSignature, commandCount, commandArgBuffer, argOffset, nullptr, countBufferOffset);
		}

		inline static void DispatchCompute(DirectXCommandBuffer* commandList, uint32_t dispatchThreadCountX, uint32_t dispatchThreadCountY, uint32_t dispatchThreadCountZ)
		{
			sRendererAPI->DispatchCompute(commandList, dispatchThreadCountX, dispatchThreadCountY, dispatchThreadCountZ);
		}

		inline static void SetViewport(const float x, const float y, const float width, const float height)
		{
			sRendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetWireframeMode(int i)
		{
			sRendererAPI->SetWireframeMode(i);
		}

		inline static void SetSwapchainRenderTarget()
		{
			sRendererAPI->SetSwapchainRenderTarget();
		}

		inline static void BlitToTexture(Texture2D* srcTx, Texture2D* dstTx)
		{
			sRendererAPI->BlitToTexture(srcTx, dstTx);
		}

		inline static void BlitToSwapchain(Texture2D* srcTx)
		{
			sRendererAPI->BlitToSwapchain(srcTx);
		}
		
		inline static void SetFinalRenderTextureId(void* textureId)
		{
			sRendererAPI->SetFinalRenderTextureId(textureId);
		}

		inline static void* GetFinalRenderTextureId()
		{
			return sRendererAPI->GetFinalRenderTextureId();
		}

		inline static uint32_t AppendBufferRegion(DirectXCommandBuffer* commandList, Buffer* dstBuffer, Buffer* srcBuffer)
		{
			return sRendererAPI->AppendBufferRegion(commandList, dstBuffer, srcBuffer);
		}

		inline static void BindPipeline(DirectXCommandBuffer* commandList, Technique* technique)
		{
			sRendererAPI->BindPipeline(commandList, technique);
		}

		inline static void BindVertexBuffer(DirectXCommandBuffer* commandList, Buffer* buffer)
		{
			sRendererAPI->BindVertexBuffer(commandList, buffer);
		}

		inline static void BindIndexBuffer(DirectXCommandBuffer* commandList, Buffer* buffer)
		{
			sRendererAPI->BindIndexBuffer(commandList,buffer);
		}

		inline static void BindUniformBuffer(DirectXCommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
		{
			sRendererAPI->BindUniformBuffer(commandList, buffer, bindslot);
		}

		inline static void BindStructuredBuffer(DirectXCommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
		{
			sRendererAPI->BindStructuredBuffer(commandList, buffer, bindslot);
		}

		inline static void SetRootConstant(DirectXCommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues)
		{
			sRendererAPI->SetRootConstant(commandList, rootParameterIndex, srcData, destOffsetIn32BitValues);
		}

		inline static void SetRootConstants(DirectXCommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* data, uint32_t destOffsetIn32BitValues)
		{
			sRendererAPI->SetRootConstants(commandList, rootParameterIndex, numValuesSet, data, destOffsetIn32BitValues);
		}

		inline static void SetConstantBufferView(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
		{
			sRendererAPI->SetConstantBufferView(commandList, bindSlot, gpuAddress);
		}

		inline static void SetShaderResourceView(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
		{
			sRendererAPI->SetShaderResourceView(commandList, bindSlot, gpuAddress);
		}

		inline static void SetUnorderedAccessView(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
		{
			sRendererAPI->SetUnorderedAccessView(commandList, bindSlot, gpuAddress);
		}

		inline static void SetRootDescriptorTable(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress)
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