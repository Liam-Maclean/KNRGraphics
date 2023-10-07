#pragma once 
#include "framebuffer.h"
#include <cassert>
#include "vulkan/vulkan.h"
#include "platform/vulkan/vulkan_util.h"


namespace Qualia
{
	class VulkanFramebuffer
		: public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { assert(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;

		VkFramebuffer m_FrameBuffer;
		Vulkan::FrameBufferAttachment m_FrameBufferAttachment;
		VkRenderPass m_RenderPass;

		////G FrameBuffer Wrapper
		//==========FOR REFERENCE===========
		//struct GFrameBuffer
		//{
		//	int32_t width, height;
		//	VkFramebuffer frameBuffer;
		//	FrameBufferAttachment position, normal, albedo, depth;
		//	VkRenderPass renderPass;
		//};
	};
}