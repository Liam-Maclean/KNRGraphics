#pragma once

#include "graphics/renderer_api.h"
#include "core/core.h"

namespace Qualia
{
	class OpenGLRendererAPI
		: public RendererAPI
	{
	public:
		virtual void Init() override;

		virtual void SetClearColor(const Vector4f& color) override;
		virtual void Clear() const override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
		virtual void SetViewport(const float x, const float y, const float width, const float height);
		virtual void SetSwapchainRenderTarget();
		virtual void SetWireframeMode(int i);
		virtual void PushState(ERenderState state);
		virtual void PopState();
		virtual void BeginRender();
		virtual void EndRender();
	};
}