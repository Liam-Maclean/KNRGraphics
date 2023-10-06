#include <qlpch.h>
#include "directx_system_manager.h"
#include <limits>
#include "graphics/systems/deferred_lighting_system.h"
#include "graphics/systems/forward_system.h"
#include "graphics/systems/fxaa_system.h"
#include "graphics/systems/shadow_system.h"
#include "graphics/systems/taa_system.h"
#include "graphics/graphics_profiler.h"

namespace QRender
{
	RenderSystemManager::RenderSystemManager()
	{
		m_gpuProfiler = QRender::GraphicsProfiler::Create();
	}

	RenderSystemManager::~RenderSystemManager()
	{
	}

	void RenderSystemManager::Resize(int width, int height)
	{
		assert(width > 0);
		assert(height > 0);

		m_width = width;
		m_height = height;
		//Resize all the subsystems 
		for (auto system : m_systems)
		{
			system->Resize(GetRenderWidth(), GetRenderHeight());
		}
	}

	void RenderSystemManager::DebugDraw()
	{
		//Resize all the subsystems 
		for (auto system : m_systems)
		{
			system->DebugDraw();
		}
	}

	void RenderSystemManager::DrawFrametimes()
	{
		m_gpuProfiler->DisplayFrameData();
	}
}