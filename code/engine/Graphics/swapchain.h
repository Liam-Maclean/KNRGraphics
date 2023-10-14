#pragma once
struct GLFWwindow;

namespace KNR
{
	class Swapchain
	{
	public:
		Swapchain();
		Swapchain(void* windowPtr, void* instance, int screenWidth, int screenHeight);
		Swapchain(GLFWwindow* hwnd, int screenWidth, int screenHeight);
		~Swapchain();

	protected:

		int width, height;
		unsigned int m_videoCardMemory;
		bool m_vsync_enabled = false, m_fullscreen = false;
		char m_videoCardDescription[128];
	};
}

