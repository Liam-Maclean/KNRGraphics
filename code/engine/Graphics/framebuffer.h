#pragma once
#include "render_types.h"

namespace KNR
{
	class Texture2D;

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(TextureFormat format)
			: textureFormat(format) {}
		FramebufferTextureSpecification(TextureFormat format, uint32_t samples)
			: textureFormat(format), samples(samples) {}
		TextureFormat textureFormat = TextureFormat::TEXTURE_FORMAT_R8G8B8A8_UNORM;
		uint32_t samples = 1;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}
		~FramebufferAttachmentSpecification() { Attachments.clear(); }
		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};
	class DirectXCommandBuffer;
	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind(DirectXCommandBuffer* commandList) = 0;
		virtual void Unbind(DirectXCommandBuffer* commandList) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		virtual Texture2D* GetFramebufferTexture(uint32_t slot) const = 0;
		virtual Texture2D* GetDepthTexture() const = 0;
		static Framebuffer* Create(const FramebufferSpecification& spec);
	};


}