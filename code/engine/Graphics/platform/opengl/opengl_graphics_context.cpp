#include "qlpch.h"

#include "opengl_graphics_context.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace QRender
{
	COpenGLContext::COpenGLContext(GLFWwindow* windowHandle)
		: window_handle(windowHandle) {}

	void COpenGLContext::Init()
	{
		glfwMakeContextCurrent(window_handle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	}

	void COpenGLContext::SwapBuffers()
	{
		//glfwWaitEvents();
		glfwPollEvents();
		glfwSwapBuffers(window_handle);
	}
}
