#include "vulkan_texture.h"
#include "vulkan/vulkan.h"
#include "graphics/platform/vulkan/vulkan_util.h"
#include "graphics/platform/vulkan/vulkan_device.h"
#include "glad/glad.h"
#include "core/core.h"
#include "logging/log.h"


namespace Qualia
{
	VulkanTexture2D::VulkanTexture2D(const char* path)
	{
		//int width, height, channels;
		////stbi_set_flip_vertically_on_load(1);
		//stbi_uc* data = stbi_load(path, &width, &height, &channels, 0);
		//if (data)
		//{
		//	m_width = width;
		//	m_height = height;
		//
		//	VkDeviceSize imageSize = m_width * m_height * 4;
		//
		//	VkBuffer stagingBuffer;
		//	VkDeviceMemory stagingBufferMemory;
		//
		//	Vulkan::Utility::GetInstance()->_CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		//
		//	void* tempdata;
		//	vkMapMemory(VulkanDevice::sVulkanDevice->GetVulkanDevice(), stagingBufferMemory, 0, imageSize, 0, &tempdata);
		//	memcpy(tempdata, data, static_cast<size_t>(imageSize));
		//	vkUnmapMemory(VulkanDevice::sVulkanDevice->GetVulkanDevice(), stagingBufferMemory);
		//	stbi_image_free(data);
		//
		//	Vulkan::Utility::GetInstance()->_CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory);
		//
		//	Vulkan::Utility::GetInstance()->_TransitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		//	Vulkan::Utility::GetInstance()->_CopyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		//	Vulkan::Utility::GetInstance()->_TransitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		//
		//	vkDestroyBuffer(VulkanDevice::sVulkanDevice->GetVulkanDevice(), stagingBuffer, nullptr);
		//	vkFreeMemory(VulkanDevice::sVulkanDevice->GetVulkanDevice(), stagingBufferMemory, nullptr);
		//}
		//else
		//{
		//	Q_CORE_ERROR("Failed to load Image at {0}", path);
		//}
		//
		//
		//int texWidth, texHeight, texChannels;
		//stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		//
		//
		////return _textureImage;

	}

	VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height)
	{

	}

	VulkanTexture2D::~VulkanTexture2D()
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