#include "directx11_graphics_context.h"
#include "directx11_texture.h"
#include "directx11_util.h"
#include "window.h"
#include "logger/logger.h"

namespace KNR
{
	CDirectX11Context::CDirectX11Context()
	{
		m_swapChain = 0;
	}
	CDirectX11Context::CDirectX11Context(const WindowDesc& windowDesc)
		: m_window(windowDesc)
	{
		m_swapChain = 0;
	}

	CDirectX11Context::~CDirectX11Context()
	{
		delete m_swapChain;
		m_swapChain = 0;
	}

	void CDirectX11Context::Init()
	{

	}

	void CDirectX11Context::Init(const WindowDesc& windowDesc)
	{
		m_window = windowDesc;

		CreateDevice();
		CreateQueues();
		CreateSwapchain();

		m_copyCommandBuffer = new DirectX11CommandBuffer(KNR::CommandBufferType::graphics);
	}

	void CDirectX11Context::SwapBuffers()
	{
		//glfwPollEvents();
	}

	void CDirectX11Context::CreateDevice()
	{
		
	}

	void CDirectX11Context::CreateQueues()
	{

	}

	void CDirectX11Context::CreateSwapchain()
	{
		//Liam fix - need IWindow size here (width and height)
		int width = m_window.width;
		int height = m_window.height;
		//m_swapChain = new VulkanSwapchain(m_window.hwnd, m_window.instance, m_window.width, m_window.height);
	}
}