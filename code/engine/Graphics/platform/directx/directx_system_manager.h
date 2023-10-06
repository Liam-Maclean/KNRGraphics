#pragma once
#include "core/core.h"
#include "graphics/platform/directx/directx_render_system.h"
#include "graphics/framebuffer.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/screen_quad.h"

#include "entt/entt.hpp"

namespace QRender
{
	class GraphicsProfiler;
	class RenderSystemManager 
	{
	public:
		RenderSystemManager();
		~RenderSystemManager();

		inline uint32_t GetViewportWidth() const { return m_width; };
		inline uint32_t GetViewportHeight() const { return m_height; };
		inline uint32_t GetRenderWidth() const { return m_width / m_upscaleRatio; }
		inline uint32_t GetRenderHeight() const { return m_height / m_upscaleRatio; }
		inline float GetUpscaleRatio() const { return m_upscaleRatio; }
		inline void SetUpscaleRatio(float inRatio) { m_upscaleRatio = inRatio; }

		template<typename System>
		Ref<System> CreateRenderSystem();
		
		template<typename System>
		Ref<System> GetRenderSystem();

		inline std::vector<Ref<RenderSystem>> GetRenderSystems() { return m_systems; }
		
		inline void SetProfilingEnabled(bool inEnabled) { m_profilingEnabled = inEnabled; }
		inline bool GetProfilingEnabled() { return m_profilingEnabled; }
		inline Ref<QRender::GraphicsProfiler> GetGraphicsProfiler() { return m_gpuProfiler; };


		void Resize(int width, int height);
		void DebugDraw();
		void DrawFrametimes();

	private:
		std::vector<Ref<RenderSystem>> m_systems;
		Ref<QRender::GraphicsProfiler> m_gpuProfiler;
		std::map<HashString, Ref<RenderSystem>> m_systemsMap;
		
		uint32_t m_width;
		uint32_t m_height;
		float m_upscaleRatio = 1.0f;
		bool m_fullscreen;
		bool m_profilingEnabled = true;
	public: 

	};

	template<typename System>
	inline Ref<System> RenderSystemManager::CreateRenderSystem()
	{
		Ref<System> system = MakeRef<System>(*this);
		HashString hash(typeid(System).name());
		m_systemsMap.emplace(hash, system);
		m_systems.push_back(system);
		return std::static_pointer_cast<System>(m_systemsMap.at(hash));
	}

	//Return system of the same type as template
	template<typename System>
	inline Ref<System> RenderSystemManager::GetRenderSystem()
	{
		HashString hs = HashString(typeid(System).name());
		auto find_it = m_systemsMap.find(hs);
		if (find_it != m_systemsMap.end())
		{
			return std::static_pointer_cast<System>(find_it->second);
		}
		return ERROR;
	}
}