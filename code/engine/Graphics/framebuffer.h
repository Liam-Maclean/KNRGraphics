#pragma once

namespace QRender
{
	class Texture2D;
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RG16,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}
		FramebufferTextureSpecification(FramebufferTextureFormat format, uint32_t samples)
			: TextureFormat(format), Samples(samples) {}
		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		uint32_t Samples = 1;
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

		virtual void Bind(Ref<DirectXCommandBuffer> commandList) = 0;
		virtual void Unbind(Ref<DirectXCommandBuffer> commandList) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		virtual Ref<Texture2D> GetFramebufferTexture(uint32_t slot) const = 0;
		virtual Ref<Texture2D> GetDepthTexture() const = 0;
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};


}