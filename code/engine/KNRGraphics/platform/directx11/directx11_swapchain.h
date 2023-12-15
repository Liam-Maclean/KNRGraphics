#pragma once
#include "swapchain.h"

namespace KNR
{
	class DirectX11Swapchain : public Swapchain
	{
	public:
		DirectX11Swapchain(void* windowPtr, void* instance, int screenWidth, int screenHeight);
		DirectX11Swapchain(void* windowPtr, int screenWidth, int screenHeight);
		~DirectX11Swapchain();
	private:
		void Initialize();
	};
}

