#pragma once
#include "render_types.h"

namespace KNR
{
	class Texture2D;

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(const TextureFormat format)
			: textureFormat(format) {}
		FramebufferTextureSpecification(const TextureFormat format, const uint32_t samples)
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

	class CommandBuffer;
	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind(CommandBuffer* commandList) = 0;
		virtual void Unbind(CommandBuffer* commandList) = 0;

		virtual void Resize(const uint32_t width, const uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID(const uint32_t index = 0) const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		inline virtual Texture2D* GetFramebufferTexture(const uint32_t slot) const = 0;
		inline virtual Texture2D* GetDepthTexture() const = 0;

		static Framebuffer* Create(const FramebufferSpecification& spec);
	};
}