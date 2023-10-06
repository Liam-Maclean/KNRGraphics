#pragma once

#include "graphics/framebuffer.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/screen_quad.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "entt/entt.hpp"

#define ScreenQuadObject (*QRender::RenderQuadSystem::GetScreenQuad())

namespace QCore
{
	class GLTFStructure;
}

namespace QRender
{
	
	class IRenderListSystem
	{
	public:
		virtual void AddModelToRenderList(Ref<QCore::GLTFStructure>& model) = 0;
		virtual void SetRenderList(std::vector<Ref<QCore::GLTFStructure>>& renderList) = 0;
	};

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
		virtual void Resize(int width, int height) = 0;
		virtual void DebugDraw() = 0;
		virtual void SetColorOutputAsFinalRenderTarget();
		virtual void SetDepthOutputAsFinalRenderTarget();
		inline Ref<QRender::Framebuffer> GetFramebuffer() { return m_framebuffer; }
	protected:
		
		RenderSystemManager& m_manager;
		Ref<QRender::Framebuffer> m_framebuffer;
		Ref<QRender::Shader> m_shader; 
		std::vector<std::function<void()>> m_recordedCallbacks;
	};

	class RenderQuadSystem : public RenderSystem
	{
	public:
		RenderQuadSystem(RenderSystemManager& manager);
		virtual ~RenderQuadSystem();
		static QRender::ScreenQuad* GetScreenQuad()
		{
			static auto screenQuad = new ScreenQuad();
			return screenQuad;
		}
	protected:
	private:

	};
}