#pragma once
#include "graphics/texture.h"
#include <string>

#include <glad/glad.h>

namespace Qualia
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const char* path);
		OpenGLTexture2D(uint32_t width, uint32_t height);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_width; }
		virtual uint32_t GetHeight() const override { return m_height; }
		virtual uint32_t GetRendererId() const override { return m_rendererID; }

		virtual void SetData(void* data, uint32_t size) override;
		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind(uint32_t slot = 0) const override;

		virtual bool operator ==(const Texture& other) const override
		{
			return m_rendererID == other.GetRendererId();
		}
	private:
		std::string m_path = "";
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		RENDERERID m_rendererID = -1;
		uint32_t m_slot = 0;
		GLenum m_internalFormat;
		GLenum m_dataFormat;
	};
}