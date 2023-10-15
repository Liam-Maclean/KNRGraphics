#pragma once

namespace KNR
{
	class VulkanFence
	{
	public:
		VulkanFence(bool copy);
		~VulkanFence();

		void Signal(VkQueue queue, VkSubmitInfo submitInfo);
		void WaitForFenceEvent();
	private:
		VkFence m_fence;
	};
}