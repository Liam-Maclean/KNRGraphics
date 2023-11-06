#include "swapchain.h"

namespace KNR
{
	Swapchain::Swapchain()
	{
		height = 0;
		width = 0;
		m_videoCardMemory = 0;
		m_videoCardDescription[127] = 'a';
	}

	Swapchain::Swapchain(void* windowPtr, void* instance, const int screenWidth, const int screenHeight)
	{
		height = screenHeight;
		width = screenWidth;
		m_videoCardMemory = 0;
		m_videoCardDescription[127] = 'a';
	}

	Swapchain::Swapchain(GLFWwindow* hwnd, const int screenWidth, const int screenHeight)
	{
		height = screenHeight;
		width = screenWidth;
		m_videoCardMemory = 0;
		m_videoCardDescription[127] = 'a';
	}

	Swapchain::~Swapchain()
	{

	}
}