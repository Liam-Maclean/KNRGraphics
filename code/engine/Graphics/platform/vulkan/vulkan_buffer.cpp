#include "graphics/platform/vulkan/vulkan_buffer.h"
#include "graphics/platform/vulkan/vulkan_device.h"
#include "graphics/platform/vulkan/vulkan_util.h"
#include "vulkan/vulkan.h"
#include "vulkan_buffer.h"

Qualia::VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
	Qualia::Vulkan::Utility::GetInstance()._CreateShaderBuffer(VulkanDevice::GetInstance().GetVulkanDevice(), size, &buffer, &memory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertices);

	descriptor.offset = 0;
	descriptor.buffer = buffer;
	descriptor.range = VK_WHOLE_SIZE;
}

Qualia::VulkanVertexBuffer::~VulkanVertexBuffer()
{
	if (buffer)
	{
		vkDestroyBuffer(device, buffer, nullptr);
	}
	if (memory)
	{
		vkFreeMemory(device, memory, nullptr);
	}
}

void Qualia::VulkanVertexBuffer::Bind() const
{
	vkBindBufferMemory(device, buffer, memory, 0);
}

void Qualia::VulkanVertexBuffer::Unbind() const
{
	vkBindBufferMemory(device, buffer, memory, 0);
}

Qualia::VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t size)
{
	Qualia::Vulkan::Utility::GetInstance()._CreateShaderBuffer(VulkanDevice::GetInstance().GetVulkanDevice(), size, &buffer, &memory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices);

	descriptor.offset = 0;
	descriptor.buffer = buffer;
	descriptor.range = VK_WHOLE_SIZE;
}

Qualia::VulkanIndexBuffer::~VulkanIndexBuffer()
{
	if (buffer)
	{
		vkDestroyBuffer(device, buffer, nullptr);
	}
	if (memory)
	{
		vkFreeMemory(device, memory, nullptr);
	}
}

void Qualia::VulkanIndexBuffer::Bind() const
{
	vkBindBufferMemory(device, buffer, memory, 0);
}

void Qualia::VulkanIndexBuffer::Unbind() const
{
	vkBindBufferMemory(device, buffer, memory, 0);
}

uint32_t Qualia::VulkanIndexBuffer::GetCount() const
{
	return count;
}



//Qualia::VulkanUniformBuffer::VulkanUniformBuffer(void * data, uint32_t size)
//{
//	Qualia::Vulkan::Utility::GetInstance()._CreateShaderBuffer(VulkanDevice::GetInstance().GetVulkanDevice(), size, &buffer, &memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, data);
//
//	descriptor.offset = 0;
//	descriptor.buffer = buffer;
//	descriptor.range = VK_WHOLE_SIZE;
//}
//
//Qualia::VulkanUniformBuffer::~VulkanUniformBuffer()
//{
//
//}
//
//void Qualia::VulkanUniformBuffer::Bind() const
//{
//}
//
//void Qualia::VulkanUniformBuffer::Unbind() const
//{
//}