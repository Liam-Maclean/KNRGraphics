#pragma once

#include "core/core.h"
#include "maths/maths.h"
struct ID3D12Resource;

namespace QRender
{

	class VertexArray;
	class Texture2D;
	class IndirectSignature;
	class Buffer;
	class Technique;
	class UniformBuffer;
	class StructuredBuffer;
	class DirectXCommandBuffer;

	enum class DrawMode
	{
		Triangles	= 0,
		Lines		= 1,
	};

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1,
			Vulkan = 2,
			DirectX = 3,
		};
	public:
		virtual ~RendererAPI() = 0 {};
		virtual void Init() = 0;

		virtual void SetClearColor(const Vector4f& color) = 0;
		virtual void Clear() const = 0;

		virtual void DrawIndexed(Ref<DirectXCommandBuffer> commandList, const Ref<VertexArray>& vertex_array, uint32_t indexCount = 0) = 0;
		virtual void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
		virtual void DrawIndirect(Ref<DirectXCommandBuffer> commandList, Ref<IndirectSignature> indirectSignature, UINT pcommandCount, Ref<QRender::Buffer> commandBuffer, UINT64 ArgumentBufferOffset, Ref<QRender::Buffer> pCountBuffer, UINT64 CountBufferOffset) = 0;

		virtual void SetViewport(const float x, const float y, const float w, const float h) = 0;
		virtual void SetWireframeMode(int i) = 0;
		virtual void SetSwapchainRenderTarget() = 0;
		virtual void PopState() = 0;
		virtual void WaitForGPU() = 0;
		virtual void SetFinalRenderTextureId(void* textureId) = 0;
		virtual void* GetFinalRenderTextureId() = 0;

		virtual void BeginRenderSilent() = 0;
		virtual void BeginRender() = 0;
		virtual void EndRender() = 0;
		virtual void BlitToTexture(Ref<Texture2D> srcTx, Ref<Texture2D> dstTx) = 0;
		virtual void BlitToSwapchain(Ref<Texture2D> srcTx) = 0;
		virtual void Present() = 0;
		virtual void DispatchCompute(Ref<DirectXCommandBuffer> commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ) = 0;

		virtual uint32_t AppendBufferRegion(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> dstBuffer, Ref<QRender::Buffer> srcBuffer) = 0;

		virtual void BindPipeline(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Technique> technique) = 0;

		virtual void BindVertexBuffer(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> buffer) = 0;
		virtual void BindIndexBuffer(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> buffer) = 0;
		virtual void BindUniformBuffer(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> buffer, uint32_t bindslot) = 0;
		virtual void BindStructuredBuffer(Ref<DirectXCommandBuffer> commandList, Ref<QRender::Buffer> buffer, uint32_t bindslot) = 0;
		

		virtual void SetRootConstant(Ref<DirectXCommandBuffer> commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) = 0;
		virtual void SetRootConstants(Ref<DirectXCommandBuffer> commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues) = 0;
		virtual void SetConstantBufferView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress) = 0;
		virtual void SetShaderResourceView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress) = 0;
		virtual void SetUnorderedAccessView(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 gpuAddress) = 0;
		virtual void SetRootDescriptorTable(Ref<DirectXCommandBuffer> commandList, uint32_t bindSlot, UINT64 startGPUAddress) = 0;
		inline static API GetAPI() { return s_target_api; }
	private:
		static API s_target_api;
	};
}