#pragma once 
#include "framebuffer.h"
#include "platform/vulkan/vulkan_graphics_context.h"
#include "platform/vulkan/vulkan_texture.h"

namespace KNR
{
	class VulkanFramebuffer
		: public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer();

		void Invalidate();

		virtual void Bind(CommandBuffer* commandList) override;
		virtual void Unbind(CommandBuffer* commandList) override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { return m_RendererID; }

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
		
		virtual Texture2D* GetFramebufferTexture(uint32_t slot) const override { return static_cast<Texture2D*>(m_framebufferTexture[slot]); }
		virtual Texture2D* GetDepthTexture() const override { return static_cast<Texture2D*>(m_framebufferDepthTexture); }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification;
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
		std::vector<VulkanTexture2D*> m_framebufferTexture;
		VulkanTexture2D* m_framebufferDepthTexture;

		bool m_resize = false;
		bool m_createOnce = false;
		bool m_srvLastFrame = false;
	};
}