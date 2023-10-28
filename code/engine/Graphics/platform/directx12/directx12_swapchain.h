#pragma once
#include "swapchain.h"
#include <d3d12.h>
#include <dxgi1_4.h>

namespace KNR
{
	class DirectX12Swapchain : public Swapchain
	{
	public:
		DirectX12Swapchain(void* windowPtr, int screenWidth, int screenHeight);
		~DirectX12Swapchain();
		IDXGISwapChain3* GetSwapchain(){return m_swapChain; }
	private:
		IDXGISwapChain3* m_swapChain;
	};
}

