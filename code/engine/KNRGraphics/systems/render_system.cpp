#include "render_commands.h"
#include "system_manager.h"
#include "render_system.h"
#include <limits>

namespace KNR
{
	RenderSystem::RenderSystem(RenderSystemManager& manager)
		: m_manager(manager)
	{

	}

	RenderSystem::~RenderSystem()
	{

	}
}