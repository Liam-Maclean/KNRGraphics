#include "vulkan_framebuffer.h"
#include <glad/glad.h>
#include <cassert>
#include <logging/log.h>

namespace Qualia
{
	static const uint32_t s_MaxFramebufferSize = 8192;

	static bool IsDepthFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
		case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
		}

		return false;
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}
		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		//vkDestroyFramebuffer()
	}

	void VulkanFramebuffer::Invalidate()
	{
		//if (m_RendererID)
		//{
		//
		//	//glDeleteFramebuffers(1, &m_RendererID);
		//	glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		//	glDeleteTextures(1, &m_DepthAttachment);
		//
		//	m_ColorAttachments.clear();
		//	m_DepthAttachment = 0;
		//}
		//
		//
		//glCreateFramebuffers(1, &m_RendererID);
		//glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		
		bool multisample = m_Specification.Samples > 1;
		
		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			//Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());
		
			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				//Utils::BindTexture(multisample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					Vulkan::Utility::GetInstance()._CreateAttachment(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &m_FrameBufferAttachment, m_Specification.Width, m_Specification.Height);
					break;
				//case FramebufferTextureFormat::RED_INTEGER:
				//	//Vulkan::Utility::_CreateAttachment(, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &m_FrameBufferAttachment, m_Specification.Width, m_Specification.Height, i);
				//	break;
				}
			}
		}
		

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				Vulkan::Utility::GetInstance()._CreateAttachment(VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, &m_FrameBufferAttachment, m_Specification.Width, m_Specification.Height);
				break;
			}
		}
	}

	void VulkanFramebuffer::Bind()
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		//glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void VulkanFramebuffer::Unbind()
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			Q_CORE_TRACE("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;
		
		Invalidate();
	}

	int VulkanFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		//assert(attachmentIndex < m_ColorAttachments.size());
		//
		//glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		//int pixelData;
		//glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		//return pixelData;
		return 0;
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		//assert(attachmentIndex < m_ColorAttachments.size());
		//
		//auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		//glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
		//	Utils::FramebufferTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
	}
}