#include <qlpch.h>
#include "shadow_system.h"
#include "assets/model_asset.h"
#include "graphics/render_commands.h"
#include "assets/asset_loader.h"
#include "graphics/platform/directx/directx_technique.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_pipeline_layout.h"
#include "graphics/graphics_profiler.h"

namespace QRender
{

	ShadowSystem::ShadowSystem(RenderSystemManager& manager)
		: RenderSystem(manager)
	{

	}

	ShadowSystem::~ShadowSystem()
	{

	}

	void ShadowSystem::Init()
	{
		Q_RENDER_INFO("INITIALISING SHADOW SYSTEM");

		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		QRender::FramebufferSpecification shadowPassSpec;
		shadowPassSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::Depth,
		};
		shadowPassSpec.Width = 4096;
		shadowPassSpec.Height = 4096;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = _countof(DirectXVertexInputLayouts::kVertexInputStaticMesh);
		inputLayoutDesc.pInputElementDescs = DirectXVertexInputLayouts::kVertexInputStaticMesh;

		//This is describing the techniques layout
		DirectXPipelineLayout pipelineLayout = {};
		pipelineLayout.AddConstantResourceView("Camera Matrices", 0, ShaderVisibility::vertex);
		pipelineLayout.AddConstantResourceView("Skinned Mesh Transforms", 1, ShaderVisibility::vertex);
		pipelineLayout.AddRootConstant("Model Transform Push Constant", 16, 2, 0, ShaderVisibility::vertex);
		pipelineLayout.AddSampler("Point Sampler", 0, SamplerType::min_mag_mip_linear);
		pipelineLayout.CreateRootSignature();

		m_vertShader = QRender::Shader::Create("assets/shaders/StaticMesh/static_shadow_vert.qvert");

		D3D12_SHADER_BYTECODE* vertBytecode = static_cast<D3D12_SHADER_BYTECODE*>(m_vertShader->GetShaderBytecode());

		//This is the whole collection of info we need to create a new technique
		PipelineCreateInfo shadowPipelineCreateInfo = {};
		shadowPipelineCreateInfo.vertShader = *vertBytecode; //Shaders would be added to the asset database instead of being made manually
		shadowPipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		shadowPipelineCreateInfo.pipelineLayout = pipelineLayout;
		shadowPipelineCreateInfo.vertexInputLayout = inputLayoutDesc; //We need a place for input layouts (per model perhaps?)
		shadowPipelineCreateInfo.wireframeModeEnabled = false;
		shadowPipelineCreateInfo.depthTestEnabled = true;
		shadowPipelineCreateInfo.cullMode = CullingMode::backface;

		m_technique = QRender::Technique::Create(shadowPipelineCreateInfo);

		m_framebuffer = QRender::Framebuffer::Create(shadowPassSpec);

		QRender::BufferDescriptor shadowCameraBufferDesc = {};
		shadowCameraBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		shadowCameraBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		shadowCameraBufferDesc.size = sizeof(CameraOrthoData);
		shadowCameraBufferDesc.debugName = L"Shadow Ortho Camera UBO";

		m_shadowCameraDataUBO = QRender::Buffer::Create(shadowCameraBufferDesc);
		Q_RENDER_INFO("FINISHED INITIALIZING SHADOW SYSTEM");
	}

	void ShadowSystem::Render()
	{
		m_commandList->Reset();
		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "Shadow System ms");

		//Shadow map pass
		m_framebuffer->Bind(m_commandList);
		{
			m_technique->Bind(m_commandList);

			//Bind shadow ortho camera
			QRender::RenderCommand::BindUniformBuffer(m_commandList, m_shadowCameraDataUBO, 0);

			//std::map< entt::entity, InstancedRenderable>::iterator it;
			//for (it = m_renderData.renderables->begin(); it != m_renderData.renderables->end(); it++)
			//{
			//	Ref<StaticMeshAsset> asset = static_pointer_cast<StaticMeshAsset>(it->second.mesh);
			//	QRender::RenderCommand::SetGraphicsRootConstants(2, 16, &it->second.transform, 0);
			//	DrawStatic(*asset->GetRef());
			//}
			//m_testModel->Draw(true);
		}
		m_framebuffer->Unbind(m_commandList);

		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void ShadowSystem::DrawStatic(const QRender::StaticMesh& staticMesh)
	{
		//staticMesh.indexBuffer->Bind(m_commandList);
		//staticMesh.vertexBuffer->Bind(m_commandList);
		m_commandList->Get()->DrawIndexedInstanced(staticMesh.indexCount, 1, 0, 0, 0);
	}

	void ShadowSystem::Update()
	{
	}

	void ShadowSystem::SetUpData()
	{

	}

	void ShadowSystem::Dispatch()
	{

	}

	void ShadowSystem::Bind()
	{

	}

	void ShadowSystem::Unbind()
	{

	}

	void ShadowSystem::DebugDraw()
	{
	}

	void ShadowSystem::Resize(int width, int height)
	{
	}

	void ShadowSystem::UpdateShaderData(RenderData renderData)
	{
		m_renderData = renderData;
		void* shadowOrthoMapAddr = m_shadowCameraDataUBO->Map();
		memcpy(shadowOrthoMapAddr, static_cast<void*>(&m_renderData.cameraData), sizeof(CameraOrthoData));
		m_shadowCameraDataUBO->UnMap();
	}

	void ShadowSystem::UpdateCameraData(CameraOrthoData inCameraData)
	{
		m_renderData.cameraData = inCameraData;
		void* shadowOrthoMapAddr = m_shadowCameraDataUBO->Map();
		memcpy(shadowOrthoMapAddr, static_cast<void*>(&m_renderData.cameraData), sizeof(CameraOrthoData));
		m_shadowCameraDataUBO->UnMap();
	}

	void ShadowSystem::SetRenderables(std::map<entt::entity, InstancedRenderable>& renderables)
	{
		m_renderData.renderables = &renderables;
	}

	//TODO Liam - To remove this
	void ShadowSystem::DrawNode(QCore::Node* node)
	{
		if (node->mesh)
		{
			//node->mesh->uniformBuffer.nodeMatrixBuffer->Bind(m_commandList, 1);
			for (int j = 0; j < node->mesh->submeshes.size(); ++j)
			{
				m_commandList->Get()->DrawIndexedInstanced(node->mesh->submeshes[j]->indexCount, 1, node->mesh->submeshes[j]->firstIndex, 0, 0);
			}
		}

		for (auto& child : node->children)
		{
			DrawNode(child);
		}
	}

	//TODO Liam - To remove this
	void ShadowSystem::Draw(QCore::GLTFStructure& gltfModel)
	{
		gltfModel.m_vertexBuffer->Bind(m_commandList);
		gltfModel.m_indexBuffer->Bind(m_commandList);

		for (auto& node : gltfModel.nodes)
		{
			DrawNode(node);
		}
	}
}