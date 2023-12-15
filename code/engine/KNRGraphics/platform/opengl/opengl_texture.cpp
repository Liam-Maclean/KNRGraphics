#include "opengl_texture.h"
#include "opengl_graphics_context.h"
#include "opengl_framebuffer.h"
#include "opengl_util.h"

namespace KNR
{
	Texture2D* Texture2D::Create(const TextureDescriptor& desc)
	{
		return new OpenglTexture2D(desc);
	}

	OpenglTexture2D::OpenglTexture2D(const TextureDescriptor& desc)
	{
		if (desc.textureType == TextureType::ASSET)
		{
			//Store the data pointer, we are going to upload this texture later
			m_data = std::move(desc.textureAsset.data);
			assert(m_data, "Failed to load image!");
		}
		else if (desc.textureType == TextureType::FRAMEBUFFER)
		{
			FramebufferSpecification* framebufferSpec = desc.textureFramebuffer.framebufferSpec;
			FramebufferTextureSpecification* framebufferTextureSpec = desc.textureFramebuffer.framebufferTextureSpec;

			m_width = framebufferSpec->Width;
			m_height = framebufferSpec->Height;

			assert(m_width > 0);
			assert(m_height > 0);
		}
	}

	void OpenglTexture2D::Upload()
	{
		//Staging buffer
	}

	OpenglTexture2D::~OpenglTexture2D()
	{

	}

	uint64_t OpenglTexture2D::GetHandle() const
	{
		return 0;
	}

	void OpenglTexture2D::ResizeResource(FramebufferTextureSpecification framebufferTextureSpec, FramebufferSpecification framebufferSpec)
	{

	}

	void OpenglTexture2D::Bind(uint32_t slot) const
	{

	}

	void OpenglTexture2D::Unbind(uint32_t slot) const
	{

	}
}