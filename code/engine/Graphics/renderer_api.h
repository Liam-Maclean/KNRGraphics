#pragma once

struct ID3D12Resource;
namespace KNR
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
			DirectX11 = 3,
			DirectX12 = 3,
		};
	public:
		virtual ~RendererAPI() = 0 {};
		virtual void Init() = 0;

		virtual void SetClearColor(const Vector4f& color) = 0;
		virtual void Clear() const = 0;
		virtual void DrawIndirect(DirectXCommandBuffer* commandList, IndirectSignature* indirectSignature, UINT pcommandCount, Buffer* commandBuffer, UINT64 ArgumentBufferOffset, Buffer* pCountBuffer, UINT64 CountBufferOffset) = 0;

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
		virtual void BlitToTexture(Texture2D* srcTx, Texture2D* dstTx) = 0;
		virtual void BlitToSwapchain(Texture2D* srcTx) = 0;
		virtual void Present() = 0;
		virtual void DispatchCompute(DirectXCommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ) = 0;

		virtual uint32_t AppendBufferRegion(DirectXCommandBuffer* commandList, Buffer* dstBuffer, Buffer* srcBuffer) = 0;

		virtual void BindPipeline(DirectXCommandBuffer* commandList, Technique* technique) = 0;

		virtual void BindVertexBuffer(DirectXCommandBuffer* commandList, Buffer* buffer) = 0;
		virtual void BindIndexBuffer(DirectXCommandBuffer* commandList, Buffer* buffer) = 0;
		virtual void BindUniformBuffer(DirectXCommandBuffer* commandList, Buffer* buffer, uint32_t bindslot) = 0;
		virtual void BindStructuredBuffer(DirectXCommandBuffer* commandList, Buffer* buffer, uint32_t bindslot) = 0;
		

		virtual void SetRootConstant(DirectXCommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) = 0;
		virtual void SetRootConstants(DirectXCommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues) = 0;
		virtual void SetConstantBufferView(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) = 0;
		virtual void SetShaderResourceView(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) = 0;
		virtual void SetUnorderedAccessView(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress) = 0;
		virtual void SetRootDescriptorTable(DirectXCommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress) = 0;
		inline static API GetAPI() { return s_target_api; }
	private:
		static API s_target_api;
	};
}