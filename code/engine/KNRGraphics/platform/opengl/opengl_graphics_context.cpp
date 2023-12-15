#include "opengl_graphics_context.h"
#include "opengl_texture.h"
#include "opengl_util.h"
#include "window.h"
#include "logger/logger.h"

namespace KNR
{
	COpenglContext::COpenglContext()
	{
		m_swapChain = 0;
	}
	COpenglContext::COpenglContext(const WindowDesc& windowDesc)
		: m_window(windowDesc)
	{
		m_swapChain = 0;
	}

	COpenglContext::~COpenglContext()
	{
		delete m_swapChain;
		m_swapChain = 0;
	}

	void COpenglContext::Init()
	{

	}

	void COpenglContext::Init(const WindowDesc& windowDesc)
	{
		m_window = windowDesc;

		CreateInstance();
		CreateDevice();
		CreateQueues();
		CreateSwapchain();

		m_copyCommandBuffer = new VulkanCommandBuffer(KNR::CommandBufferType::graphics);
	}

	void COpenglContext::SwapBuffers()
	{
		//glfwPollEvents();
	}

	void COpenglContext::CreateDevice()
	{
		
	}

	void COpenglContext::CreateInstance()
	{
		
	}

	void COpenglContext::CreateQueues()
	{
		
	}

	void COpenglContext::CreateSwapchain()
	{
		//Liam fix - need IWindow size here (width and height)
		int width = m_window.width;
		int height = m_window.height;
		//m_swapChain = new VulkanSwapchain(m_window.hwnd, m_window.instance, m_window.width, m_window.height);
	}
}