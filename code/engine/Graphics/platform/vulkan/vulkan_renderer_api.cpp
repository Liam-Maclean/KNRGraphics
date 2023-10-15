#include "vulkan_buffer.h"
#include "vulkan_graphics_context.h"
#include "vulkan_renderer_api.h"
#include "vulkan_texture.h"
#include "vulkan_technique.h"
#include <basetsd.h>

namespace KNR
{
	VulkanRendererAPI::~VulkanRendererAPI()
	{

	}

	void VulkanRendererAPI::Initialize()
	{
		//Initializes Vulkan Context
		VulkanContext;

		m_width = 1920;
		m_height = 1080;
	}

	void VulkanRendererAPI::SetClearColor(float r, float g, float b, float a)
	{

	}

	void VulkanRendererAPI::Clear() const
	{

	}

	uint32_t VulkanRendererAPI::AppendBufferRegion(CommandBuffer* commandList, KNR::Buffer* dstBuffer, KNR::Buffer* srcBuffer)
	{
	
	}

	void VulkanRendererAPI::BindPipeline(CommandBuffer* commandList, KNR::Technique* technique)
	{
		
	}

	void VulkanRendererAPI::BindVertexBuffer(CommandBuffer* commandList, Buffer* buffer)
	{
		
	}

	void VulkanRendererAPI::BindIndexBuffer(CommandBuffer* commandList, Buffer* buffer)
	{
		
	}

	void VulkanRendererAPI::BindUniformBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		
	}

	void VulkanRendererAPI::BindStructuredBuffer(CommandBuffer* commandList, Buffer* buffer, uint32_t bindslot)
	{
		
	}

	void VulkanRendererAPI::DrawIndexedInstanced(CommandBuffer* commandList, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t baseVertexLocation, uint32_t startInstanceLocation)
	{

	}

	void VulkanRendererAPI::DrawIndirect(CommandBuffer* commandList, IndirectSignature* indirectSignature, UINT commandCount, Buffer* argumentBuffer, UINT64 argumentBufferOffset, KNR::Buffer* countBuffer, UINT64 countBufferOffset)
	{

	}

	void VulkanRendererAPI::DispatchCompute(CommandBuffer* commandList, uint32_t dispatchGroupCountX, uint32_t dispatchGroupCountY, uint32_t dispatchGroupCountZ)
	{

	}

	void VulkanRendererAPI::SetViewport(const float x, const float y, const float width, const float height)
	{
		
	}

	void VulkanRendererAPI::SetWireframeMode(int i)
	{

	}

	void VulkanRendererAPI::RecordCommandBuffers()
	{
	
	}


	void VulkanRendererAPI::BeginRenderSilent()
	{

	}

	//Geometry render begin
	void VulkanRendererAPI::BeginRender()
	{
		
	}

	//Geometry render end
	void VulkanRendererAPI::EndRender()
	{
		
	}

	void VulkanRendererAPI::Present()
	{
		
	}


	void VulkanRendererAPI::SetSwapchainRenderTarget()
	{
		
	}

	void VulkanRendererAPI::SetFinalRenderTextureId(void* textureId)
	{
		m_framebuffer = textureId;
	}

	void* VulkanRendererAPI::GetFinalRenderTextureId()
	{
		return m_framebuffer;
	}

	void VulkanRendererAPI::WaitForGPU()
	{
		
	}

	void VulkanRendererAPI::BlitToTexture(Texture2D* srcTx, Texture2D* dstTx)
	{
		
	}

	void VulkanRendererAPI::BlitToSwapchain(Texture2D* srcTx)
	{
		
	}

	void VulkanRendererAPI::SetRootConstant(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues)
	{
		
	}

	void VulkanRendererAPI::SetRootConstants(CommandBuffer* commandList, uint32_t rootParameterIndex, uint32_t numValuesSet, void* srcData, uint32_t destOffsetIn32BitValues)
	{
		
	}

	void VulkanRendererAPI::SetConstantBufferView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		
	}

	void VulkanRendererAPI::SetShaderResourceView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		
	}

	void VulkanRendererAPI::SetUnorderedAccessView(CommandBuffer* commandList, uint32_t bindSlot, UINT64 gpuAddress)
	{
		
	}

	void VulkanRendererAPI::SetRootDescriptorTable(CommandBuffer* commandList, uint32_t bindSlot, UINT64 startGPUAddress)
	{
		
	}

	void VulkanRendererAPI::WaitForPreviousFrame()
	{
		
	}

	void VulkanRendererAPI::CreateCPUHeaps()
	{
		
	}

	void VulkanRendererAPI::CreateRenderTargets()
	{
		
	}
}