#pragma once
#include "vulkan_graphics_context.h"
#include "texture.h"
#include <string>

namespace KNR
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const TextureDescriptor& desc);
		virtual ~VulkanTexture2D();

		virtual void Upload() override;
		virtual uint32_t GetWidth() const override { return m_width; }
		virtual uint32_t GetHeight() const override { return m_height; }
		virtual uint32_t GetRenderId() const override { return m_rendererID; }
		virtual uint32_t GetEditorRenderId() const override { return m_editorID; }
		virtual uint64_t GetHandle() const override;
		
		virtual void ResizeResource(FramebufferTextureSpecification framebufferTextureSpec, FramebufferSpecification framebufferSpec);

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind(uint32_t slot = 0) const override;

		VkImage GetTextureHandle() { return m_image; }

		virtual bool operator ==(const Texture& other) const override
		{
			return m_rendererID == other.GetRenderId();
		}

	private:
		VkImage m_image;
		VkImageView m_imageView;
		VkFramebuffer m_framebuffer;

		std::vector<std::string> m_cubemapFilenames;

		std::string m_path = "";
		int m_width = 0;
		int m_height = 0;
		int m_channels = 0;
		void* m_data = nullptr;

		uint32_t m_framebufferRTVHeapIndex = 0;
		uint32_t m_framebufferDSVHeapIndex = 0;
		uint32_t m_rendererID;
		uint32_t m_editorID;
	};
}