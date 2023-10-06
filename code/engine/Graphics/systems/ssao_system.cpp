#include <qlpch.h>
#include "ssao_system.h"
#include "graphics/render_commands.h"

#include "graphics/platform/directx/directx_technique.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_pipeline_layout.h"
#include "graphics/platform/directx/directx_indirect_signature.h"
#include "graphics/graphics_profiler.h"

namespace QRender
{

	SSAOSystem::SSAOSystem(RenderSystemManager& manager)
		: RenderSystem(manager)
	{

	}

	SSAOSystem::~SSAOSystem()
	{

	}

	void SSAOSystem::Init()
	{
		Q_RENDER_INFO("INITIALISING SCREEN SPACED AMBIENT OCCLUSION (SSAO) SYSTEM");

		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		//Create framebuffer for the pass
		QRender::FramebufferSpecification forwardAccumulationPassSpec;
		forwardAccumulationPassSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::RGBA8,
			QRender::FramebufferTextureFormat::DEPTH24STENCIL8,
		};
		forwardAccumulationPassSpec.Width = m_manager.GetRenderWidth();
		forwardAccumulationPassSpec.Height = m_manager.GetRenderHeight();
		m_framebuffer = QRender::Framebuffer::Create(forwardAccumulationPassSpec);

		Q_RENDER_INFO("FINISHED INITIALIZING SCREEN SPACED AMBIENT OCCLUSION (SSAO) SYSTEM");
	}

	void SSAOSystem::Render()
	{
		m_commandList->Reset();
		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "Screen spaced ambient occlusion (SSAO) ms");

		//Shadow map pass
		m_framebuffer->Bind(m_commandList);
		{
			//Bind technique (Post process)
			m_technique->Bind(m_commandList);
		}
		m_framebuffer->Unbind(m_commandList);
		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void SSAOSystem::Update()
	{
	}

	void SSAOSystem::SetUpData()
	{

	}

	void SSAOSystem::Dispatch()
	{

	}

	void SSAOSystem::Bind()
	{

	}

	void SSAOSystem::Unbind()
	{

	}

	void SSAOSystem::DebugDraw()
	{
	}

	void SSAOSystem::Resize(int width, int height)
	{
	}
}