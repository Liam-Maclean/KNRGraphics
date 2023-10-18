#include "vulkan_framebuffer.h"
#include "vulkan_util.h"
#include "render_commands.h"

namespace KNR
{
	Framebuffer* Framebuffer::Create(const FramebufferSpecification& spec)
	{
		return new VulkanFramebuffer(spec);
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Util::IsDepthFormat(spec.textureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		if (m_ColorAttachmentSpecifications.size())
		{

		}

		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{

	}

	void VulkanFramebuffer::Invalidate()
	{
		//Reset these heap sizes to overwrite the previous ones if this is a resize
		if (m_RendererID)
		{
			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		bool multisample = m_Specification.Samples > 1;
		
		if (m_framebufferTexture.size())
		{
			if (m_ColorAttachmentSpecifications.size())
			{
				m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
				for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); ++i)
				{
					//Release the original resource and resize the SRV and RTV
					//m_framebufferTexture[i]->ResizeResource(m_ColorAttachmentSpecifications[i], m_Specification);
					//m_framebufferTexture[i]->RecreateRTV(&m_RTVHeap);

					m_RendererID = m_framebufferTexture[i]->GetEditorRenderId();
				}
			}
		}
		else
		{
			// Attachments
			if (m_ColorAttachmentSpecifications.size())
			{
				m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
				for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); ++i)
				{
					KNR::TextureDescriptor textureDesc = {};
					textureDesc.textureType = TextureType::FRAMEBUFFER;
					textureDesc.textureUsage = TextureUsage::RENDERTARGET;
					textureDesc.textureFramebuffer.framebufferSpec = &m_Specification;
					textureDesc.textureFramebuffer.framebufferTextureSpec = &m_ColorAttachmentSpecifications[i];
					textureDesc.debugName = L"Color Render Target";

					//m_framebufferTexture.push_back(reinterpret_cast<DirectXTexture2D*>(Texture2D::Create(textureDesc)));
					m_RendererID = m_framebufferTexture[i]->GetEditorRenderId();
					//m_framebufferTexture[i]->CreateRTV(&m_RTVHeap);
				}
			}
		}
		
		if (m_framebufferDepthTexture)
		{

			//m_framebufferDepthTexture->ResizeResource(m_DepthAttachmentSpecification, m_Specification);
			//m_framebufferDepthTexture->RecreateDSV(&m_DSVHeap);
			m_DepthAttachment = 1;
			
		}
		else
		{

			KNR::TextureDescriptor textureDesc = {};
			textureDesc.textureType = TextureType::FRAMEBUFFER;
			textureDesc.textureUsage = TextureUsage::RENDERTARGET;
			textureDesc.textureFramebuffer.framebufferSpec = &m_Specification;
			textureDesc.textureFramebuffer.framebufferTextureSpec = &m_DepthAttachmentSpecification;
			textureDesc.debugName = L"Depth Render Target";

			//m_framebufferDepthTexture = reinterpret_cast<DirectXTexture2D*>(Texture2D::Create(textureDesc));
			//m_framebufferDepthTexture->CreateDSV(&m_DSVHeap);

			m_DepthAttachment = 1;
		}

		if (!m_createOnce)
		{
			m_createOnce = true;
		}
		m_srvLastFrame = true;
	}

	void VulkanFramebuffer::Bind(CommandBuffer* commandList)
	{
		
	}

	void VulkanFramebuffer::Unbind(CommandBuffer* commandList)
	{
	
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}

	int VulkanFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		int pixelData = 0;
		return pixelData;
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{

	}
}