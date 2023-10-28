#pragma once
#include "swapchain.h"

namespace KNR
{
	class OpenglSwapchain : public Swapchain
	{
	public:
		OpenglSwapchain(void* windowPtr, void* instance, int screenWidth, int screenHeight);
		OpenglSwapchain(void* windowPtr, int screenWidth, int screenHeight);
		~OpenglSwapchain();
	private:
	};
}

