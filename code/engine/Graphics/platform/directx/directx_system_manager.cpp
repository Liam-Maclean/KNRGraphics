#include "directx_system_manager.h"
#include "graphics_profiler.h"
#include <limits>

namespace KNR
{
	RenderSystemManager::RenderSystemManager()
	{
		m_gpuProfiler = KNR::GraphicsProfiler::Create();
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