#include <qlpch.h>
#include "dof_system.h"
#include "graphics/render_commands.h"

#include "graphics/platform/directx/directx_technique.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_pipeline_layout.h"
#include "graphics/platform/directx/directx_indirect_signature.h"
#include "graphics/graphics_profiler.h"

namespace QRender
{

	DOFSystem::DOFSystem(RenderSystemManager& manager)
		: RenderSystem(manager)
	{

	}

	DOFSystem::~DOFSystem()
	{

	}

	void DOFSystem::Init()
	{
		Q_RENDER_INFO("INITIALISING DEPTH OF FIELD SYSTEM");

		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		//Create framebuffer for the pass
		QRender::FramebufferSpecification dofPassSpec;
		dofPassSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::RGBA8,
		};
		dofPassSpec.Width = m_manager.GetRenderWidth();
		dofPassSpec.Height = m_manager.GetRenderHeight();
		m_framebuffer = QRender::Framebuffer::Create(dofPassSpec);

		Q_RENDER_INFO("FINISHED INITIALIZING DEPTH OF FIELD SYSTEM");
	}

	void DOFSystem::Render()
	{
		m_commandList->Reset();
		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "Depth of Field System ms");

		//Shadow map pass
		m_framebuffer->Bind(m_commandList);
		{
			//bind technique (post process)
			m_technique->Bind(m_commandList);
		}
		m_framebuffer->Unbind(m_commandList);
		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void DOFSystem::Update()
	{
	}

	void DOFSystem::SetUpData()
	{

	}

	void DOFSystem::Dispatch()
	{

	}

	void DOFSystem::Bind()
	{

	}

	void DOFSystem::Unbind()
	{

	}

	void DOFSystem::DebugDraw()
	{
	}

	void DOFSystem::Resize(int width, int height)
	{
	}
}