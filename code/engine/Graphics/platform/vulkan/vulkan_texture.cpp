#include "vulkan_texture.h"
#include "vulkan_graphics_context.h"
#include "vulkan_framebuffer.h"
#include "vulkan_util.h"

namespace KNR
{
	Texture2D* Texture2D::Create(const TextureDescriptor& desc)
	{
		return new VulkanTexture2D(desc);
	}

	VulkanTexture2D::VulkanTexture2D(const TextureDescriptor& desc)
	{
		if (desc.textureType == TextureType::ASSET)
		{
			//Just load the asset, nothing special. Upload the texture later to prevent threading issues with command lists
			m_data = stbi_load(desc.textureAsset.path, &m_width, &m_height, &m_channels, 4);
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

			//Create a default resource description for the texture
			//Liam fix - Lets deal with resource descriptions later
			VkDevice device = VulkanContext.GetDevice();

			VkImageCreateInfo imageCreateInfo = {};
			VkFormat imageFormat = {};
			VkImageAspectFlags imageAspectFlags = {};
			//we need to handle depth texture buffers slightly differently
			if (framebufferTextureSpec->TextureFormat == FramebufferTextureFormat::DEPTH24STENCIL8)
			{
				imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
				imageCreateInfo.extent.width = m_width;
				imageCreateInfo.extent.height = m_height;
				imageCreateInfo.extent.depth = 1;
				imageCreateInfo.mipLevels = 1;
				imageCreateInfo.arrayLayers = 1;
				imageCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
				imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

				Util::ErrorCheck(vkCreateImage(device, &imageCreateInfo, nullptr, &m_image));
			}
			//Standard RTVC 
			else
			{
				imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
				imageCreateInfo.extent.width = m_width;
				imageCreateInfo.extent.height = m_height;
				imageCreateInfo.extent.depth = 1;
				imageCreateInfo.mipLevels = 1;
				imageCreateInfo.arrayLayers = 1;
				imageCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
				imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

				//Move to a enum converter function
				switch (framebufferTextureSpec->TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
					break;
				case FramebufferTextureFormat::RG16:
					imageCreateInfo.format = VK_FORMAT_R16G16_SFLOAT;
					break;
				case  FramebufferTextureFormat::RED_INTEGER:
					imageCreateInfo.format = VK_FORMAT_R32_SFLOAT;
					break;
				default:
					break;
				}
				Util::ErrorCheck(vkCreateImage(device, &imageCreateInfo, nullptr, &m_image));
			}
			

			//Store this image view for later for the pipeline stage
			VkImageViewCreateInfo view_info = {};
			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.image = m_image;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = imageFormat;
			view_info.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			view_info.subresourceRange.aspectMask = imageAspectFlags;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;

			Util::ErrorCheck(vkCreateImageView(device, &view_info, nullptr, &m_imageView));
		}
	}

	void VulkanTexture2D::Upload()
	{
		
	}

	VulkanTexture2D::~VulkanTexture2D()
	{

	}

	uint64_t VulkanTexture2D::GetHandle() const
	{
		return 0;
	}

	void VulkanTexture2D::ResizeResource(FramebufferTextureSpecification framebufferTextureSpec, FramebufferSpecification framebufferSpec)
	{

	}

	void VulkanTexture2D::SetData(void* data, uint32_t size)
	{

	}

	void VulkanTexture2D::Bind(uint32_t slot) const
	{

	}

	void VulkanTexture2D::Unbind(uint32_t slot) const
	{

	}
}