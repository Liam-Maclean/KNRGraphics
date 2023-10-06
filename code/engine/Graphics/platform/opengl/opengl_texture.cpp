#include "opengl_texture.h"
#include "glad/glad.h"
#include "core/core.h"
#include "logging/log.h"

#include "utilities/stb_image.hpp"

namespace Qualia
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		return MakeRef<OpenGLTexture2D>(width, height);
	}

	Ref<Texture2D> Texture2D::Create(const char* path)
	{
		return MakeRef<OpenGLTexture2D>(path);		
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_width(width), m_height(height)
	{
		m_internalFormat = GL_RGBA8;
		m_dataFormat = GL_RGBA;
#if defined(OPENGL)
		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
							  
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif
		Initialise();
	}

	OpenGLTexture2D::OpenGLTexture2D(const char* path)
		: m_path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			data = stbi_load(path, &width, &height, &channels, 0);
		}
		assert(data, "Failed to load image!");

		m_width = width;
		m_height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_internalFormat = internalFormat;
		m_dataFormat = dataFormat;
#if defined(OPENGL)
		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D( m_rendererID, 1, internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
							  
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);
#endif
		stbi_image_free(data);

		Initialise();
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
#if defined(OPENGL)
		glDeleteTextures(1, &m_rendererID);
#endif
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_dataFormat == GL_RGBA ? 4 : 3;
		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_rendererID);
	}

	void OpenGLTexture2D::Unbind(uint32_t slot) const
	{
		glBindTextureUnit(slot, 0);
	}
}