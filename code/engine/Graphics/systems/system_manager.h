#pragma once
#include "render_system.h"
#include "framebuffer.h"
#include "shader.h"
#include "texture.h"
#include "screen_quad.h"

namespace KNR
{
	class GraphicsProfiler;
	class RenderSystemManager final
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
		System* CreateRenderSystem();
		
		template<typename System>
		System* GetRenderSystem();

		inline std::vector<RenderSystem*> GetRenderSystems() { return m_systems; }
		
		inline void SetProfilingEnabled(bool inEnabled) { m_profilingEnabled = inEnabled; }
		inline bool GetProfilingEnabled() { return m_profilingEnabled; }
		inline GraphicsProfiler* GetGraphicsProfiler() { return m_gpuProfiler; };


		void Resize(int width, int height);
		void DebugDraw();
		void DrawFrametimes();

	private:
		std::vector<RenderSystem*> m_systems;
		KNR::GraphicsProfiler* m_gpuProfiler;
		std::map<uint32_t, RenderSystem*> m_systemsMap;
		
		uint32_t m_width;
		uint32_t m_height;
		float m_upscaleRatio = 1.0f;
		bool m_fullscreen;
		bool m_profilingEnabled = true;
	public: 

	};

	template<typename System>
	inline System* RenderSystemManager::CreateRenderSystem()
	{
		System* system = new System(*this);
		uint32_t hs = std::hash(typeid(System).name());
		m_systemsMap.emplace(hs, system);
		m_systems.push_back(system);
		return reinterpret_cast<System*>(m_systemsMap.at(hs));
	}

	//Return system of the same type as template
	template<typename System>
	inline System* RenderSystemManager::GetRenderSystem()
	{
		uint32_t hs = std::hash(typeid(System).name());
		auto find_it = m_systemsMap.find(hs);
		if (find_it != m_systemsMap.end())
		{
			return reinterpret_cast<System*>(find_it->second);
		}
		return ERROR;
	}
}