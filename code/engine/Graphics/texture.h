#pragma once
#include "framebuffer.h"

namespace KNR
{
	enum class TextureUsage
	{
		SHADERRESOURCE = 0x1, //Shader visible
		RENDERTARGET = 0x2,	//Render target flag
		UNORDEREDACCESS = 0x4,//Compute/write access
	};

	enum class TextureType
	{
		ASSET,		//Comes from a file
		FRAMEBUFFER,//Created as a framebuffer
		CUBEMAP,	//Created as a cubemap
	};

	struct TextureCubemapDesc
	{
		const char* path;
		uint32_t width;
		uint32_t height;
	};

	struct TextureAssetDesc 
	{
		const char* path;
		void* data;
		int width, height, bitdepth;
	};

	struct TextureFramebufferDesc 
	{
		FramebufferSpecification* framebufferSpec;
		FramebufferTextureSpecification* framebufferTextureSpec;
	};

	struct TextureDescriptor
	{
		TextureUsage textureUsage;
		TextureType textureType;
		union
		{
			TextureAssetDesc textureAsset;
			TextureFramebufferDesc textureFramebuffer;
			TextureCubemapDesc textureCubemap;
		};
		LPCWSTR debugName;
	};

	class Texture
	{
	public:

		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRenderId() const = 0;
		virtual uint32_t GetEditorRenderId() const = 0;
		virtual uint64_t GetHandle() const = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind(uint32_t slot = 0) const = 0;
		virtual bool operator==(const Texture& other) const = 0;
		virtual void Upload() = 0;
		virtual void ResizeResource(FramebufferTextureSpecification framebufferTextureSpec, FramebufferSpecification framebufferSpec) = 0;
	protected:
		virtual void Initialise() {}
	};


	class Texture2D 
		: public Texture
	{
	public:
		static Texture2D* Create(const TextureDescriptor& desc);
	};
}
