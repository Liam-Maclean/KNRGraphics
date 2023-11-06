#pragma once
struct GLFWwindow;

namespace KNR
{
	class Swapchain
	{
	public:
		Swapchain();
		Swapchain(void* windowPtr, void* instance, const int screenWidth, const int screenHeight);
		Swapchain(GLFWwindow* hwnd, const int screenWidth, const int screenHeight);
		~Swapchain();

	protected:

		int width, height;
		unsigned int m_videoCardMemory;
		bool m_vsync_enabled = false, m_fullscreen = false;
		char m_videoCardDescription[128];
	};
}

