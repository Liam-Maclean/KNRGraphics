
#include <cassert>
#include "d3dx12.h"
#include "directx12_util.h"
#include "directx12_framebuffer.h"
#include "directx12_frame_heap.h"


namespace KNR
{
	Framebuffer* Framebuffer::Create(const FramebufferSpecification& spec)
	{
		return new DirectX12Framebuffer(spec);
	}

	static const uint32_t s_MaxFramebufferSize = 8192;

	DirectX12Framebuffer::DirectX12Framebuffer(const FramebufferSpecification& spec)
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
			m_RTVHeap.Create(DirectX12Context.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_ColorAttachmentSpecifications.size());
		}

		m_DSVHeap.Create(DirectX12Context.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		Invalidate();
	}

	DirectX12Framebuffer::~DirectX12Framebuffer()
	{

	}

	void DirectX12Framebuffer::Invalidate()
	{
		//Reset these heap sizes to overwrite the previous ones if this is a resize
		ID3D12Device* device = DirectX12Context.GetDevice();
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

					m_framebufferTexture.push_back(reinterpret_cast<DirectX12Texture2D*>(Texture2D::Create(textureDesc)));
					m_RendererID = m_framebufferTexture[i]->GetEditorRenderId();
					m_framebufferTexture[i]->CreateRTV(&m_RTVHeap);
				}
			}
		}
		
		if (m_framebufferDepthTexture)
		{
			if (m_DepthAttachmentSpecification.textureFormat != TextureFormat::TEXTURE_FORMAT_INVALID)
			{
				m_framebufferDepthTexture->ResizeResource(m_DepthAttachmentSpecification, m_Specification);
				m_framebufferDepthTexture->RecreateDSV(&m_DSVHeap);
				m_DepthAttachment = 1;
			}
		}
		else
		{
			if (m_DepthAttachmentSpecification.textureFormat != TextureFormat::TEXTURE_FORMAT_INVALID)
			{
				KNR::TextureDescriptor textureDesc = {};
				textureDesc.textureType = TextureType::FRAMEBUFFER;
				textureDesc.textureUsage = TextureUsage::RENDERTARGET;
				textureDesc.textureFramebuffer.framebufferSpec = &m_Specification;
				textureDesc.textureFramebuffer.framebufferTextureSpec = &m_DepthAttachmentSpecification;
				textureDesc.debugName = L"Depth Render Target";

				m_framebufferDepthTexture = reinterpret_cast<DirectX12Texture2D*>(Texture2D::Create(textureDesc));
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

	void DirectX12Framebuffer::Bind(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandBuffer = static_cast<DirectX12CommandBuffer*>(commandList);
		DirectX12FrameHeap* frameHeap = DirectX12Context.GetFrameHeap();
		//Starts the frame, initializing the descriptor table index to 0
		frameHeap->StartFrame(directXCommandBuffer);

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
				directXCommandBuffer->Get()->ResourceBarrier(resourceToRTVBarriers.size(), resourceToRTVBarriers.data());
			}
		}

		//We dont want to clear the render target here, only alter what it currently contains

		if (m_DepthAttachment && m_framebufferTexture.size() < 1)
		{
			directXCommandBuffer->Get()->OMSetRenderTargets(0, nullptr, true, &m_DSVHeap.hCPUHeapStart);
		}
		else if (m_DepthAttachment)
		{
			directXCommandBuffer->Get()->OMSetRenderTargets(m_framebufferTexture.size(), &m_RTVHeap.hCPUHeapStart, true, &m_DSVHeap.hCPUHeapStart);
		}
		else
		{
			directXCommandBuffer->Get()->OMSetRenderTargets(m_framebufferTexture.size(), &m_RTVHeap.hCPUHeapStart, true, NULL);
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

		directXCommandBuffer->Get()->RSSetViewports(1, &viewport);				//RS = Rasterizer
		directXCommandBuffer->Get()->RSSetScissorRects(1, &scissorRect);

		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		//Clear the color attachment list
		for (int i = 0; i < m_framebufferTexture.size(); ++i)
		{
			directXCommandBuffer->Get()->ClearRenderTargetView(m_RTVHeap.handleCPU(i), clearColor, 0, nullptr);
		}

		//Clear depth, we need to do this to see something
		if (m_DepthAttachment)
		{
			directXCommandBuffer->Get()->ClearDepthStencilView(m_DSVHeap.handleCPU(0), D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0xff, 0, nullptr);
		}
	}

	void DirectX12Framebuffer::Unbind(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandBuffer = static_cast<DirectX12CommandBuffer*>(commandList);
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
			directXCommandBuffer->Get()->ResourceBarrier(rtvToResourceBarriers.size(), rtvToResourceBarriers.data());
		}
		m_srvLastFrame = true;
	}

	void DirectX12Framebuffer::Resize(const uint32_t width, const uint32_t height)
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
}