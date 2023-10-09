#include "render_commands.h"
#include "directx_system_manager.h"
#include "directx_render_system.h"
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

	RenderQuadSystem::RenderQuadSystem(RenderSystemManager& manager)
		: RenderSystem(manager)
	{

	}

	RenderQuadSystem::~RenderQuadSystem()
	{

	}

	//Override this if you're outputting depth instead of color 
	void RenderSystem::SetColorOutputAsFinalRenderTarget()
	{
		uint64_t texId = m_framebuffer->GetColorAttachmentRendererID();
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = DirectXContext.GetImGuiHeap()->handleGPU(texId);
		KNR::RenderCommand::SetFinalRenderTextureId(reinterpret_cast<void*>(textureHandle.ptr));
	}

	void RenderSystem::SetDepthOutputAsFinalRenderTarget()
	{
		//TODO
	}
}