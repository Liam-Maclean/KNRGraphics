#include "opengl_renderer_api.h"

#include "opengl_vertex_array.h"
#include "glad/glad.h"
#include "logging/log.h"

namespace Qualia
{
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         Q_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       Q_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          Q_CORE_WARN(message);  return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: Q_CORE_TRACE(message); return;
		}

		assert(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
#ifdef _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		//glFrontFace(GL_CCW);
	}

	void OpenGLRendererAPI::SetClearColor(const Vector4f& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	
	void OpenGLRendererAPI::Clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		// shader bind
		vertexArray->Bind();
		uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::SetViewport(const float x, const float y, const float width, const float height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetSwapchainRenderTarget()
	{
	}

	void OpenGLRendererAPI::SetWireframeMode(int i)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void OpenGLRendererAPI::PushState(ERenderState state)
	{
		switch (state)
		{
		default: 
			break;
		}
	}

	void OpenGLRendererAPI::PopState()
	{
	}
	
	void OpenGLRendererAPI::BeginRender()
	{
	}
	void OpenGLRendererAPI::EndRender()
	{
	}
}