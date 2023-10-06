#pragma once
#include "graphics/swapchain.h"
#include <d3d12.h>
#include <dxgi1_4.h>
class GLFWWindow;
namespace QRender
{
	class DirectXSwapchain : public Swapchain
	{
	public:
		DirectXSwapchain(void* windowPtr, int screenWidth, int screenHeight);
		~DirectXSwapchain();
		IDXGISwapChain3* GetSwapchain(){return m_swapChain; }
	private:
		IDXGISwapChain3* m_swapChain;

	};
}

