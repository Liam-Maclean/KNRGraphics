#pragma once

namespace KNR
{
	class VulkanFence
	{
	public:
		VulkanFence(bool copy);
		~VulkanFence();

		void IncrementFenceValue();
		void Signal();
		void Signal(ID3D12CommandQueue* queue);
		void WaitForFenceEvent();
		void SetToCurrentFenceValue();
	private:
		unsigned long long m_fenceValue;
		HANDLE m_fenceEvent;
		ID3D12Fence* m_fence;
		bool m_copyQueue;
	};
}