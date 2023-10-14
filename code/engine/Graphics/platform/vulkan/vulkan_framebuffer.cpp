#include "vulkan_framebuffer.h"
#include <cassert>

namespace KNR
{
	Framebuffer* Framebuffer::Create(const FramebufferSpecification& spec)
	{
		return new VulkanFramebuffer(spec);
	}

	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils {
		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{

		}

		static void BindTexture(bool multisampled, uint32_t id)
		{

		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		if (m_ColorAttachmentSpecifications.size())
		{
			m_RTVHeap.Create(DirectXContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_ColorAttachmentSpecifications.size());
		}

		m_DSVHeap.Create(DirectXContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{

	}

	void VulkanFramebuffer::Invalidate()
	{
		//Reset these heap sizes to overwrite the previous ones if this is a resize
		ID3D12Device* device = DirectXContext.GetDevice();
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
				for (int i = 0; i < m_ColorAttachmentSpecifications.size(); ++i)
				{
					//Release the original resource and resize the SRV and RTV
					m_framebufferTexture[i]->ResizeResource(m_ColorAttachmentSpecifications[i], m_Specification);
					m_framebufferTexture[i]->RecreateRTV(&m_RTVHeap);

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
				for (int i = 0; i < m_ColorAttachmentSpecifications.size(); ++i)
				{
					KNR::TextureDescriptor textureDesc = {};
					textureDesc.textureType = TextureType::FRAMEBUFFER;
					textureDesc.textureUsage = TextureUsage::RENDERTARGET;
					textureDesc.textureFramebuffer.framebufferSpec = &m_Specification;
					textureDesc.textureFramebuffer.framebufferTextureSpec = &m_ColorAttachmentSpecifications[i];
					textureDesc.debugName = L"Color Render Target";

					m_framebufferTexture.push_back(reinterpret_cast<DirectXTexture2D*>(Texture2D::Create(textureDesc)));
					m_RendererID = m_framebufferTexture[i]->GetEditorRenderId();
					m_framebufferTexture[i]->CreateRTV(&m_RTVHeap);
				}
			}
		}
		
		if (m_framebufferDepthTexture)
		{
			if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
			{
				m_framebufferDepthTexture->ResizeResource(m_DepthAttachmentSpecification, m_Specification);
				m_framebufferDepthTexture->RecreateDSV(&m_DSVHeap);
				m_DepthAttachment = 1;
			}
		}
		else
		{
			if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
			{
				KNR::TextureDescriptor textureDesc = {};
				textureDesc.textureType = TextureType::FRAMEBUFFER;
				textureDesc.textureUsage = TextureUsage::RENDERTARGET;
				textureDesc.textureFramebuffer.framebufferSpec = &m_Specification;
				textureDesc.textureFramebuffer.framebufferTextureSpec = &m_DepthAttachmentSpecification;
				textureDesc.debugName = L"Depth Render Target";

				m_framebufferDepthTexture = reinterpret_cast<DirectXTexture2D*>(Texture2D::Create(textureDesc));
				m_framebufferDepthTexture->CreateDSV(&m_DSVHeap);

				m_DepthAttachment = 1;
			}
		}

		if (!m_createOnce)
		{
			m_createOnce = true;
		}
		m_srvLastFrame = true;
	}

	void VulkanFramebuffer::Bind(DirectXCommandBuffer* commandList)
	{
		DirectXFrameHeap* frameHeap = DirectXContext.GetFrameHeap();
		//Starts the frame, initializing the descriptor table index to 0
		frameHeap->StartFrame(commandList);

		//if we transitioned to a shader resource view, we need to get the texture back into a render target for the next frame
		if (m_srvLastFrame || m_resize)
		{
			//Liam - Group transition barriers together or we'll have cache misses
			//We can also group depth since we are needing them all at the same time
			std::vector<D3D12_RESOURCE_BARRIER> resourceToRTVBarriers;
			if (m_framebufferTexture.size() != 0)
			{
				for (int i = 0; i < m_framebufferTexture.size(); ++i)
				{
					resourceToRTVBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(m_framebufferTexture[i]->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
				}
			}
			if (m_DepthAttachment)
			{
				resourceToRTVBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(m_framebufferDepthTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
			}

			if (resourceToRTVBarriers.size() != 0)
			{
				commandList->Get()->ResourceBarrier(resourceToRTVBarriers.size(), resourceToRTVBarriers.data());
			}
		}

		//We dont want to clear the render target here, only alter what it currently contains

		if (m_DepthAttachment && m_framebufferTexture.size() < 1)
		{
			commandList->Get()->OMSetRenderTargets(0, nullptr, true, &m_DSVHeap.hCPUHeapStart);
		}
		else if (m_DepthAttachment)
		{
			commandList->Get()->OMSetRenderTargets(m_framebufferTexture.size(), &m_RTVHeap.hCPUHeapStart, true, &m_DSVHeap.hCPUHeapStart);
		}
		else
		{
			commandList->Get()->OMSetRenderTargets(m_framebufferTexture.size(), &m_RTVHeap.hCPUHeapStart, true, NULL);
		}
		

		D3D12_VIEWPORT viewport;							//What we see in the output of the rasterizer
		D3D12_RECT scissorRect;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = m_Specification.Width;
		viewport.Height = m_Specification.Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		scissorRect.left = 0;
		scissorRect.top = 0;
		scissorRect.right = m_Specification.Width;
		scissorRect.bottom = m_Specification.Height;

		commandList->Get()->RSSetViewports(1, &viewport);				//RS = Rasterizer
		commandList->Get()->RSSetScissorRects(1, &scissorRect);

		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		//Clear the color attachment list
		for (int i = 0; i < m_framebufferTexture.size(); ++i)
		{
			commandList->Get()->ClearRenderTargetView(m_RTVHeap.handleCPU(i), clearColor, 0, nullptr);
		}

		//Clear depth, we need to do this to see something
		if (m_DepthAttachment)
		{
			commandList->Get()->ClearDepthStencilView(m_DSVHeap.handleCPU(0), D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0xff, 0, nullptr);
		}
	}

	void VulkanFramebuffer::Unbind(DirectXCommandBuffer* commandList)
	{
		std::vector<D3D12_RESOURCE_BARRIER> rtvToResourceBarriers;
		if (m_framebufferTexture.size() != 0)
		{
			for (int i = 0; i < m_framebufferTexture.size(); ++i)
			{
				rtvToResourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(m_framebufferTexture[i]->GetTextureHandle(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
			}
		}
		if (m_DepthAttachment)
		{
			rtvToResourceBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(m_framebufferDepthTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		}

		if (rtvToResourceBarriers.size() != 0)
		{
			commandList->Get()->ResourceBarrier(rtvToResourceBarriers.size(), rtvToResourceBarriers.data());
		}
		m_srvLastFrame = true;
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			assert(0);
			return;
		}
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