#include "directx11_buffer.h"
#include "directx11_graphics_context.h"
#include "directx11_renderer_api.h"
#include "directx11_texture.h"
#include "directx11_technique.h"
#include "logger/logger.h"
#include <basetsd.h>

namespace KNR
{
	DirectX11RendererAPI::~DirectX11RendererAPI()
	{

	}

	void DirectX11RendererAPI::Initialize(const WindowDesc& windowDesc)
	{
		//Initialize the in-build KNR logger
		KNR::Logger::Init();

		//Initializes Vulkan Context
		DirectX11Context.Init(windowDesc);

		m_width = 1920;
		m_height = 1080;
	}

	void DirectX11RendererAPI::SetClearColor(float r, float g, float b, float a)
	{

	}

	void DirectX11RendererAPI::Clear() const
	{

	}

	uint32_t DirectX11RendererAPI::AppendBufferRegion(CommandBuffer* commandList, KNR::Buffer* dstBuffer, KNR::Buffer* srcBuffer)
	{
		uint32_t offset;
		return offset;
	}

	void DirectX11RendererAPI::BindPipeline(CommandBuffer* commandList, KNR::Technique* technique)
	{
		
	}

	void DirectX11RendererAPI::BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer)
	{
		
	}

	void DirectX11RendererAPI::BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer)
	{
		
	}

	void DirectX11RendererAPI::BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		
	}

	void DirectX11RendererAPI::BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		
	}

	void DirectX11RendererAPI::DrawIndexedInstanced(CommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
	{

	}

	void DirectX11RendererAPI::DrawIndirect(CommandBuffer* commandList, IndirectSignature* indirectSignature, UINT commandCount, Buffer* argumentBuffer, UINT64 argumentBufferOffset, KNR::Buffer* countBuffer, UINT64 countBufferOffset)
	{

	}

	void DirectX11RendererAPI::DispatchCompute(CommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ)
	{

	}

	void DirectX11RendererAPI::SetViewport(const float x, const float y, const float width, const float height)
	{
		
	}

	void DirectX11RendererAPI::SetScissor(const float left, const float top, const float right, const float bottom)
	{

	}

	void DirectX11RendererAPI::SetWireframeMode(int i)
	{

	}

	void DirectX11RendererAPI::RecordCommandBuffers()
	{
	
	}


	void DirectX11RendererAPI::BeginRenderSilent()
	{

	}

	//Geometry render begin
	void DirectX11RendererAPI::BeginRender()
	{
		
	}

	//Geometry render end
	void DirectX11RendererAPI::EndRender()
	{
		
	}

	void DirectX11RendererAPI::Present()
	{
		
	}


	void DirectX11RendererAPI::SetSwapchainRenderTarget()
	{
		
	}

	void DirectX11RendererAPI::SetFinalRenderTextureId(void* textureId)
	{
		m_framebuffer = textureId;
	}

	void* DirectX11RendererAPI::GetFinalRenderTextureId()
	{
		return m_framebuffer;
	}

	void DirectX11RendererAPI::WaitForGPU()
	{
		
	}

	void DirectX11RendererAPI::BeginCommandList()
	{
	}

	void DirectX11RendererAPI::SubmitCommandBufferImmediate(CommandBuffer* commandList)
	{
	}

	void DirectX11RendererAPI::SubmitCommandBuffer(CommandBuffer* commandList)
	{
	}

	void DirectX11RendererAPI::WaitOnCommandList(CommandBuffer* commandList)
	{
	}

	void DirectX11RendererAPI::BlitToTexture(Texture2D* srcTx, Texture2D* dstTx)
	{
		
	}

	void DirectX11RendererAPI::BlitToSwapchain(Texture2D* srcTx)
	{
		
	}

	void DirectX11RendererAPI::SetRootConstant(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues)
	{
		
	}

	void DirectX11RendererAPI::SetRootConstants(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues)
	{
		
	}

	void DirectX11RendererAPI::SetConstantBufferView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		
	}

	void DirectX11RendererAPI::SetShaderResourceView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		
	}

	void DirectX11RendererAPI::SetUnorderedAccessView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		
	}

	void DirectX11RendererAPI::SetRootDescriptorTable(CommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress)
	{
		
	}

	void DirectX11RendererAPI::WaitForPreviousFrame()
	{
		
	}

	void DirectX11RendererAPI::CreateCPUHeaps()
	{
		
	}

	void DirectX11RendererAPI::CreateRenderTargets()
	{
		
	}
}