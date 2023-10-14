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

	Swapchain::Swapchain(void* windowPtr, void* instance, int screenWidth, int screenHeight)
	{
		height = screenHeight;
		width = screenWidth;
		m_videoCardMemory = 0;
		m_videoCardDescription[127] = 'a';
	}

	Swapchain::Swapchain(GLFWwindow* hwnd, int screenWidth, int screenHeight)
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