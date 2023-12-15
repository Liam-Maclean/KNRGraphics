#pragma once

/// <summary>
/// Swapchain
/// 
/// A swapchain used to present the backbuffer to the main output window
/// End user should never get access to this
/// Works with GLFW, and HWND Win32/UWP windows
/// </summary>

struct GLFWwindow;
namespace KNR
{
	class Swapchain
	{
	public:
		~Swapchain() = default;

		static Swapchain* Create(void* windowPtr, void* instance, const int screenWidth, const int screenHeight);
		static Swapchain* Create(GLFWwindow* hwnd, const int screenWidth, const int screenHeight);

	protected:

		int width, height;
		unsigned int m_videoCardMemory;
		bool m_vsync_enabled = false, m_fullscreen = false;
		char m_videoCardDescription[128];
	};
}