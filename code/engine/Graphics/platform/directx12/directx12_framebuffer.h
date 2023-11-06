#pragma once 
#include "framebuffer.h"
#include "directx12_heap.h"
#include "directx12_graphics_context.h"
#include "directx12_texture.h"


namespace KNR
{
	class DirectX12CommandBuffer;
	class DirectX12Framebuffer final
		: public Framebuffer
	{
	public:
		DirectX12Framebuffer(const FramebufferSpecification& spec);
		~DirectX12Framebuffer();

		void Invalidate();

		void Bind(CommandBuffer* commandList) override;
		void Unbind(CommandBuffer* commandList) override;

		void Resize(const uint32_t width, const uint32_t height) override;
		int ReadPixel(const uint32_t attachmentIndex, const int x, const int y) override;

		void ClearAttachment(const uint32_t attachmentIndex, const int value) override;

		uint32_t GetColorAttachmentRendererID(const uint32_t index = 0) const override { return m_RendererID; }

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
		
		inline Texture2D* GetFramebufferTexture(const uint32_t slot) const override { return static_cast<Texture2D*>(m_framebufferTexture[slot]); }
		inline Texture2D* GetDepthTexture() const override { return static_cast<Texture2D*>(m_framebufferDepthTexture); }
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
		DirectX12Heap m_RTVHeap;
		DirectX12Heap m_DSVHeap;
		std::vector<DirectX12Texture2D*> m_framebufferTexture;
		DirectX12Texture2D* m_framebufferDepthTexture;

		bool m_resize = false;
		bool m_createOnce = false;
		bool m_srvLastFrame = false;
	};
}