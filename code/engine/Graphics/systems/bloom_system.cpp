#include <qlpch.h>
#include "bloom_system.h"
#include "graphics/render_commands.h"

#include "graphics/platform/directx/directx_technique.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_pipeline_layout.h"
#include "graphics/graphics_profiler.h"

namespace QRender
{
	BloomSystem::BloomSystem(RenderSystemManager& manager)
		: RenderSystem(manager)
	{

	}

	BloomSystem::~BloomSystem()
	{

	}

	void BloomSystem::Init()
	{
		Q_RENDER_INFO("INITIALISING BLOOM SYSTEM");

		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		//Create framebuffer for the pass
		QRender::FramebufferSpecification bloomPassSpec;
		bloomPassSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::RGBA8,
		};
		bloomPassSpec.Width = m_manager.GetRenderWidth();
		bloomPassSpec.Height = m_manager.GetRenderHeight();
		m_framebuffer = QRender::Framebuffer::Create(bloomPassSpec);

		//TODO

		Q_RENDER_INFO("FINISHED INITIALIZING BLOOM SYSTEM");
	}

	void BloomSystem::Render()
	{
		m_commandList->Reset();
		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "Bloom System ms");

		//Bloom pass
		m_framebuffer->Bind(m_commandList);
		{
			//Bind Technique (Post Process)
			m_technique->Bind(m_commandList);
			
			//Screen quad render (post process)
		}
		m_framebuffer->Unbind(m_commandList);
		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void BloomSystem::Update()
	{
	}

	void BloomSystem::SetUpData()
	{

	}

	void BloomSystem::Dispatch()
	{

	}

	void BloomSystem::Bind()
	{

	}

	void BloomSystem::Unbind()
	{

	}

	void BloomSystem::DebugDraw()
	{
	}

	void BloomSystem::Resize(int width, int height)
	{
	}
}