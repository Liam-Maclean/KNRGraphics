#include <qlpch.h>
#include "frustum_cull_compute_system.h"
#include "graphics/render_commands.h"
#include "graphics/camera.h"
#include "graphics/systems/forward_system.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_pipeline.h"
#include "graphics/platform/directx/directx_technique.h"

namespace QRender
{

	FrustumCullComputeSystem::FrustumCullComputeSystem(RenderSystemManager& manager)
		: RenderSystem(manager)
	{

	}
	FrustumCullComputeSystem::~FrustumCullComputeSystem()
	{

	}

	void FrustumCullComputeSystem::Init()
	{
		Q_RENDER_INFO("INITIALIZING FRUSTUM CULL (COMPUTE) RENDER SYSTEM");

		m_computeCommandList = MakeRef<DirectXCommandBuffer>(CommandBufferType::compute);
		m_computeCommandList->Close();
		m_computeShader = QRender::Shader::Create("assets/shaders/computer/frustum_cull.qcomp");

		//This is describing the techniques layout
		DirectXPipelineLayout pipelineLayout = {};
		pipelineLayout.AddConstantResourceView("Cull data push constants", 0, ShaderVisibility::all);
		pipelineLayout.AddShaderResourceView("Object data structured buffer", 0, ShaderVisibility::all);
		pipelineLayout.AddShaderResourceView("Draw commands structured buffer", 1, ShaderVisibility::all);
		pipelineLayout.AddShaderResourceView("Draw instances structured buffer", 2, ShaderVisibility::all);
		pipelineLayout.CreateRootSignature();

		D3D12_SHADER_BYTECODE* computeBytecode = static_cast<D3D12_SHADER_BYTECODE*>(m_computeShader->GetShaderBytecode());
		PipelineCreateInfo computePipelineCreateInfo = {};
		computePipelineCreateInfo.computeShader = *computeBytecode; //Shaders would be added to the asset database instead of being made manually
		computePipelineCreateInfo.pipelineLayout = pipelineLayout;

		m_technique = QRender::Technique::Create(computePipelineCreateInfo);
		m_renderData.cullData = new CullData();
	}

	void FrustumCullComputeSystem::Render()
	{
		//Just incase someone uses render instead of dispatch
		Dispatch();
	}

	void FrustumCullComputeSystem::Update()
	{

	}

	void FrustumCullComputeSystem::SetUpData()
	{

	}

	void FrustumCullComputeSystem::Dispatch()
	{
		m_computeCommandList->Reset();

		m_technique->Bind(m_computeCommandList);

		if (m_renderData.renderBucket != nullptr)
		{
			float nearPlane = m_cullingCamera->GetNear();
			float farPlane = m_cullingCamera->GetFar();
			glm::mat4 view = m_cullingCamera->GetView();
			glm::mat4 proj = m_cullingCamera->GetProjection();
			glm::mat4 projT = glm::transpose(proj);
			glm::vec4 frustumX = QMaths::NormalisePlane(proj[3] + proj[0]);
			glm::vec4 frustumY = QMaths::NormalisePlane(proj[3] + proj[1]);
			view = glm::transpose(view);
			
			m_renderData.cullData->view = view;
			m_renderData.cullData->frustum[0] = frustumX.x;
			m_renderData.cullData->frustum[1] = frustumX.z;
			m_renderData.cullData->frustum[2] = frustumY.y;
			m_renderData.cullData->frustum[3] = frustumY.z;
			m_renderData.cullData->drawCount = static_cast<uint32_t>(m_renderData.renderBucket->flatBatches.size());
			m_renderData.cullData->zfar = farPlane;
			m_renderData.cullData->znear = nearPlane;
			

			//m_renderData.renderBucket->gpuIndirectCommandBuffer->Bind(m_computeCommandList, 2);


			QRender::RenderCommand::DispatchCompute(m_computeCommandList, static_cast<uint32_t>((m_renderData.renderBucket->flatBatches.size() / 256) + 1), 1, 1);
		}

		

	

		//Submit work immediately (queue is deferred from type)
		//LIAM - We should do this work asynchronously 
		m_computeCommandList->SubmitWorkImmediate();

		//dispatch
	}

	void FrustumCullComputeSystem::Bind()
	{

	}

	void FrustumCullComputeSystem::Unbind()
	{

	}

	void FrustumCullComputeSystem::DebugDraw()
	{

	}

	void FrustumCullComputeSystem::Resize(int width, int height)
	{
		//Not applicable (Compute)
	}

	void FrustumCullComputeSystem::UpdateCamera(Ref<QRender::EditorCamera> cullingCamera)
	{
		m_cullingCamera = cullingCamera;
	}

	void FrustumCullComputeSystem::SetRenderBucket(RenderBucket* inRenderBucket)
	{
		m_renderData.renderBucket = inRenderBucket;
	}
}