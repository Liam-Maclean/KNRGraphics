#pragma once
#include "framebuffer.h"

namespace KNR
{
	enum TextureUsage
	{
		ShaderResource =	1 << 0, //Shader visible
		RenderTarget =		1 << 1,	//Render target flag
		UnorderedAccess =	1 << 2,	//Compute/write access
	};

	enum TextureType
	{
		Asset =			1 << 0,	//Comes from a file
		Framebuffer =	1 << 1,	//Created as a framebuffer
		Cubemap =		1 << 2,	//Created as a cubemap
	};

	struct TextureDescriptor
	{
		TextureUsage textureUsage;
		TextureType textureType;
		TextureFormat textureFormat;
		uint32_t width, height, bitDepth, samples, mipCount;
		void* initialData;
		LPCWSTR debugName;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;
		inline virtual uint32_t GetWidth() const = 0;
		inline virtual uint32_t GetHeight() const = 0;
		inline virtual uint32_t GetRenderId() const = 0;
		inline virtual uint32_t GetEditorRenderId() const = 0;
		inline virtual uint64_t GetHandle() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
		virtual void Upload() = 0;
	protected:
		virtual void Initialise() {}
	};


	class Texture2D 
		: public Texture
	{
	public:
		static Texture2D* Create(const TextureDescriptor& desc);

	protected:
		TextureType m_TextureType;
		TextureUsage m_TextureUsage;
		TextureFormat m_TextureFormat;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_BitDepth;
		uint32_t m_Samples;
		uint32_t m_MipCount;
	};
}
