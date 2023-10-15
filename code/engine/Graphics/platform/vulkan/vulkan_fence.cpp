#include "vulkan_fence.h"
#include "vulkan_graphics_context.h"

namespace KNR
{
	VulkanFence::VulkanFence(bool copy)
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		vkCreateFence(VulkanContext.GetDevice(), &fenceInfo, nullptr, &m_fence);
	}

	VulkanFence::~VulkanFence()
	{
		vkDestroyFence(VulkanContext.GetDevice(), m_fence, nullptr);
	}

	void VulkanFence::Signal(VkQueue queue, VkSubmitInfo submitInfo)
	{
		vkQueueSubmit(queue, 1, &submitInfo, m_fence);
	}

	void VulkanFence::WaitForFenceEvent()
	{
		VkDevice device = VulkanContext.GetDevice();
		vkWaitForFences(device, 1, &m_fence, true, 0);

		vkResetFences(device, 1, &m_fence);
	}
}
