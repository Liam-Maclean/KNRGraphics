#pragma once
#include <d3d12.h>
namespace QRender
{
	class DirectXFence
	{
	public:
		DirectXFence(bool copy);
		~DirectXFence();

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