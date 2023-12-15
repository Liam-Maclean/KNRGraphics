#pragma once

#include "shader.h"
#include "texture.h"
#include <functional>

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
		virtual void Resize(const int width, const int height) = 0;
		virtual void DebugDraw() = 0;
	protected:
		
		RenderSystemManager& m_manager;
		Shader* m_shader; 
		std::vector<std::function<void()>> m_recordedCallbacks;
	};
}