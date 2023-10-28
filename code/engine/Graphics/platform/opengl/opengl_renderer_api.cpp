
#include "opengl_renderer_api.h"
#include "logger/logger.h"
#include <basetsd.h>

namespace KNR
{
	OpenGLRendererAPI::~OpenGLRendererAPI()
	{

	}

	void OpenGLRendererAPI::Initialize(const WindowDesc& windowDesc)
	{
		//Initialize the in-build KNR logger
		KNR::Logger::Init();

		//Initializes Vulkan Context

		m_width = 1920;
		m_height = 1080;
	}

	void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
	{

	}

	void OpenGLRendererAPI::Clear() const
	{

	}

	uint32_t OpenGLRendererAPI::AppendBufferRegion(CommandBuffer* commandList, KNR::Buffer* dstBuffer, KNR::Buffer* srcBuffer)
	{
		uint32_t offset;
		return offset;
	}

	void OpenGLRendererAPI::BindPipeline(CommandBuffer* commandList, KNR::Technique* technique)
	{
		
	}

	void OpenGLRendererAPI::BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer)
	{
		
	}

	void OpenGLRendererAPI::BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer)
	{
		
	}

	void OpenGLRendererAPI::BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		
	}

	void OpenGLRendererAPI::BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		
	}

	void OpenGLRendererAPI::DrawIndexedInstanced(CommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
	{

	}

	void OpenGLRendererAPI::DrawIndirect(CommandBuffer* commandList, IndirectSignature* indirectSignature, UINT commandCount, Buffer* argumentBuffer, UINT64 argumentBufferOffset, KNR::Buffer* countBuffer, UINT64 countBufferOffset)
	{

	}

	void OpenGLRendererAPI::DispatchCompute(CommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ)
	{

	}

	void OpenGLRendererAPI::SetViewport(const float x, const float y, const float width, const float height)
	{
		
	}

	void OpenGLRendererAPI::SetScissor(const float left, const float top, const float right, const float bottom)
	{

	}

	void OpenGLRendererAPI::SetWireframeMode(int i)
	{

	}

	void OpenGLRendererAPI::RecordCommandBuffers()
	{
	
	}


	void OpenGLRendererAPI::BeginRenderSilent()
	{

	}

	//Geometry render begin
	void OpenGLRendererAPI::BeginRender()
	{
		
	}

	//Geometry render end
	void OpenGLRendererAPI::EndRender()
	{
		
	}

	void OpenGLRendererAPI::Present()
	{
		
	}


	void OpenGLRendererAPI::SetSwapchainRenderTarget()
	{
		
	}

	void OpenGLRendererAPI::SetFinalRenderTextureId(void* textureId)
	{
		m_framebuffer = textureId;
	}

	void* OpenGLRendererAPI::GetFinalRenderTextureId()
	{
		return m_framebuffer;
	}

	void OpenGLRendererAPI::WaitForGPU()
	{
		
	}

	void OpenGLRendererAPI::BeginCommandList()
	{
	}

	void OpenGLRendererAPI::SubmitCommandBufferImmediate(CommandBuffer* commandList)
	{
	}

	void OpenGLRendererAPI::SubmitCommandBuffer(CommandBuffer* commandList)
	{
	}

	void OpenGLRendererAPI::WaitOnCommandList(CommandBuffer* commandList)
	{
	}

	void OpenGLRendererAPI::BlitToTexture(Texture2D* srcTx, Texture2D* dstTx)
	{
		
	}

	void OpenGLRendererAPI::BlitToSwapchain(Texture2D* srcTx)
	{
		
	}

	void OpenGLRendererAPI::SetRootConstant(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues)
	{
		
	}

	void OpenGLRendererAPI::SetRootConstants(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues)
	{
		
	}

	void OpenGLRendererAPI::SetConstantBufferView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		
	}

	void OpenGLRendererAPI::SetShaderResourceView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		
	}

	void OpenGLRendererAPI::SetUnorderedAccessView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		
	}

	void OpenGLRendererAPI::SetRootDescriptorTable(CommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress)
	{
		
	}

	void OpenGLRendererAPI::WaitForPreviousFrame()
	{
		
	}

	void OpenGLRendererAPI::CreateCPUHeaps()
	{
		
	}

	void OpenGLRendererAPI::CreateRenderTargets()
	{
		
	}
}