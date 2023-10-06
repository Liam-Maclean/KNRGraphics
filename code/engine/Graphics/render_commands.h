#pragma once

#include "renderer_api.h"

namespace QRender
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

		inline static void DrawIndexed(Ref<DirectXCommandBuffer> commandList, Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			sRendererAPI->DrawIndexed(commandList, vertexArray, indexCount);
		}

		inline static void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			sRendererAPI->DrawArrays(vertexArray, vertexCount);
		}

		inline static void DrawLines(Ref<VertexArray>& vertexArray, uint32_t vertexCount = 0)
		{
			sRendererAPI->DrawLines(vertexArray, vertexCount);
		}

		inline static void IndirectDraw(Ref<DirectXCommandBuffer> commandList, Ref<IndirectSignature> indirectSignature, uint32_t commandCount, Ref<QRender::Buffer> commandArgBuffer, uint32_t argOffset, Ref<QRender::Buffer> countBuffer, uint32_t countBufferOffset)
		{
			sRendererAPI->DrawIndirect(commandList, indirectSignature, commandCount, commandArgBuffer, argOffset, nullptr, countBufferOffset);
		}

		inline static void DispatchCompute(Ref<DirectXCommandBuffer> commandList, uint32_t dispatchThreadCountX, uint32_t dispatchThreadCountY, uint32_t dispatchThreadCountZ)
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

		inline static void BlitToTexture(Ref<Texture2D> srcTx, Ref<Texture2D> dstTx)
		{
			sRendererAPI->BlitToTexture(srcTx, dstTx);
		}

		inline static void BlitToSwapchain(Ref<Texture2D> srcTx)
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

		inline static uint32_t AppendBufferRegion(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> dstBuffer, Ref<QRender::Buffer> srcBuffer)
		{
			return sRendererAPI->AppendBufferRegion(commandList, dstBuffer, srcBuffer);
		}

		inline static void BindPipeline(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Technique> technique)
		{
			sRendererAPI->BindPipeline(commandList, technique);
		}

		inline static void BindVertexBuffer(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> buffer)
		{
			sRendererAPI->BindVertexBuffer(commandList, buffer);
		}

		inline static void BindIndexBuffer(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> buffer)
		{
			sRendererAPI->BindIndexBuffer(commandList,buffer);
		}

		inline static void BindUniformBuffer(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> buffer, uint32_t bindslot)
		{
			sRendererAPI->BindUniformBuffer(commandList, buffer, bindslot);
		}

		inline static void BindStructuredBuffer(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> buffer, uint32_t bindslot)
		{
			sRendererAPI->BindStructuredBuffer(commandList, buffer, bindslot);
		}

		inline static void SetRootConstant(Ref<DirectXCommandBuffer> commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues)
		{
			sRendererAPI->SetRootConstant(commandList, rootParameterIndex, srcData, destOffsetIn32BitValues);
		}

		inline static void SetRootConstants(Ref<DirectXCommandBuffer> commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* data, uint32_t destOffsetIn32BitValues)
		{
			sRendererAPI->SetRootConstants(commandList, rootParameterIndex, numValuesSet, data, destOffsetIn32BitValues);
		}

		inline static void SetConstantBufferView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress)
		{
			sRendererAPI->SetConstantBufferView(commandList, bindSlot, gpuAddress);
		}

		inline static void SetShaderResourceView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress)
		{
			sRendererAPI->SetShaderResourceView(commandList, bindSlot, gpuAddress);
		}

		inline static void SetUnorderedAccessView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress)
		{
			sRendererAPI->SetUnorderedAccessView(commandList, bindSlot, gpuAddress);
		}

		inline static void SetRootDescriptorTable(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 startGPUAddress)
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