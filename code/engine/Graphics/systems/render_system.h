#pragma once

#include "framebuffer.h"
#include "shader.h"
#include "texture.h"
#include "screen_quad.h"
#include <functional>

#define ScreenQuadObject (*KNR::RenderQuadSystem::GetScreenQuad())

namespace KNR
{
	class RenderSystemManager;
	class RenderSystem 
	{
	public:
		RenderSystem(RenderSystemManager& manager);
		virtual ~RenderSystem();

		virtual void Init() = 0;
		virtual void Render() = 0;
		virtual void Update() = 0;
		virtual void SetUpData() = 0;
		virtual void Dispatch() = 0;
		virtual void Bind() = 0; 
		virtual void Unbind() = 0;
		virtual void Resize(const int width, const int height) = 0;
		virtual void DebugDraw() = 0;
		virtual void SetColorOutputAsFinalRenderTarget();
		virtual void SetDepthOutputAsFinalRenderTarget();
		inline Framebuffer* GetFramebuffer() const { return m_framebuffer; }
	protected:
		
		RenderSystemManager& m_manager;
		Framebuffer* m_framebuffer;
		Shader* m_shader; 
		std::vector<std::function<void()>> m_recordedCallbacks;
	};

	class RenderQuadSystem : public RenderSystem
	{
	public:
		RenderQuadSystem(RenderSystemManager& manager);
		virtual ~RenderQuadSystem();
		static KNR::ScreenQuad* GetScreenQuad()
		{
			static auto screenQuad = new ScreenQuad();
			return screenQuad;
		}
	protected:
	private:

	};
}