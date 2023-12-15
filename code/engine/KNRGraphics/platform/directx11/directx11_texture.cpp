#include "directx11_texture.h"
#include "directx11_graphics_context.h"
#include "directx11_framebuffer.h"
#include "directx11_util.h"

namespace KNR
{
	Texture2D* Texture2D::Create(const TextureDescriptor& desc)
	{
		return new DirectX11Texture2D(desc);
	}

	DirectX11Texture2D::DirectX11Texture2D(const TextureDescriptor& desc)
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

	void DirectX11Texture2D::Upload()
	{
		//Staging buffer
	}

	DirectX11Texture2D::~DirectX11Texture2D()
	{

	}

	uint64_t DirectX11Texture2D::GetHandle() const
	{
		return 0;
	}

	void DirectX11Texture2D::ResizeResource(FramebufferTextureSpecification framebufferTextureSpec, FramebufferSpecification framebufferSpec)
	{

	}

	void DirectX11Texture2D::Bind(uint32_t slot) const
	{

	}

	void DirectX11Texture2D::Unbind(uint32_t slot) const
	{

	}
}