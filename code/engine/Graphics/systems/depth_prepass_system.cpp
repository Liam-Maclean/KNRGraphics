#include <qlpch.h>
#include "depth_prepass_system.h"
#include "assets/model_asset.h"
#include "graphics/render_commands.h"
#include "assets/asset_loader.h"

#include "graphics/systems/forward_system.h"

#include "graphics/platform/directx/directx_technique.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_pipeline_layout.h"
#include "graphics/platform/directx/directx_indirect_signature.h"
#include "graphics/graphics_profiler.h"

namespace QRender
{

	DepthPrepassSystem::DepthPrepassSystem(RenderSystemManager& manager)
		: RenderSystem(manager)
	{

	}

	DepthPrepassSystem::~DepthPrepassSystem()
	{

	}

	void DepthPrepassSystem::Init()
	{
		Q_RENDER_INFO("INITIALISING DEPTH PREPASS SYSTEM");

		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		QRender::FramebufferSpecification depthPrepassSpec;
		depthPrepassSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::RGBA8,	//Normals
			QRender::FramebufferTextureFormat::Depth,
		};
		depthPrepassSpec.Width = m_manager.GetRenderWidth();
		depthPrepassSpec.Height = m_manager.GetRenderHeight();

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = _countof(DirectXVertexInputLayouts::kVertexInputStaticMesh);
		inputLayoutDesc.pInputElementDescs = DirectXVertexInputLayouts::kVertexInputStaticMesh;


		//This is describing the techniques layout
		DirectXPipelineLayout pipelineLayout = {};
		pipelineLayout.AddConstantResourceView("", 0, ShaderVisibility::all);
		pipelineLayout.AddDescriptorTable("Matrices", 0, 7, 0, DescriptorTableType::srv, ShaderVisibility::all);
		pipelineLayout.AddDescriptorTable("Bindless Textures", 7, -1, 1, DescriptorTableType::srv, ShaderVisibility::all);
		pipelineLayout.AddConstantResourceView("Skinned matrices data", 1, ShaderVisibility::all);
		pipelineLayout.AddConstantResourceView("Material data", 2, ShaderVisibility::all);
		pipelineLayout.AddConstantResourceView("Directional Light", 3, ShaderVisibility::all);
		pipelineLayout.AddConstantResourceView("Push Constants", 4, ShaderVisibility::all);
		pipelineLayout.AddConstantResourceView("Per frame params", 6, ShaderVisibility::all);
		pipelineLayout.AddConstantResourceView("Bindless Transform Matrices", 7, ShaderVisibility::all);
		pipelineLayout.AddRootConstant("Matrix index push constant", 1, 0, 3, ShaderVisibility::vertex);
		pipelineLayout.AddSampler("Point Sampler", 0, SamplerType::min_mag_mip_linear);
		pipelineLayout.AddSampler("Anisotropic Sampler", 1, SamplerType::anisotropic);
		pipelineLayout.CreateRootSignature();


		//Depth and normal only pass
		m_vertShader = QRender::Shader::Create("assets/shaders/Quad/quad_vert.qvert");
		m_fragShader = QRender::Shader::Create("assets/shaders/Deferred/deferred_lighting_frag.qfrag");

		D3D12_SHADER_BYTECODE* vertBytecode = static_cast<D3D12_SHADER_BYTECODE*>(m_vertShader->GetShaderBytecode());
		D3D12_SHADER_BYTECODE* fragBytecode = static_cast<D3D12_SHADER_BYTECODE*>(m_fragShader->GetShaderBytecode());

		//This is the whole collection of info we need to create a new technique
		PipelineCreateInfo depthPrepassPipelineCreateInfo = {};
		depthPrepassPipelineCreateInfo.vertShader = *vertBytecode; //Shaders would be added to the asset database instead of being made manually
		depthPrepassPipelineCreateInfo.vertShader = *fragBytecode; //Shaders would be added to the asset database instead of being made manually
		depthPrepassPipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		depthPrepassPipelineCreateInfo.pipelineLayout = pipelineLayout;
		depthPrepassPipelineCreateInfo.vertexInputLayout = inputLayoutDesc; //We need a place for input layouts (per model perhaps?)
		depthPrepassPipelineCreateInfo.wireframeModeEnabled = false;
		depthPrepassPipelineCreateInfo.depthTestEnabled = true;
		depthPrepassPipelineCreateInfo.cullMode = CullingMode::backface;
		depthPrepassPipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM); //Normals (XYZ)
		m_technique = QRender::Technique::Create(depthPrepassPipelineCreateInfo);
		m_framebuffer = QRender::Framebuffer::Create(depthPrepassSpec);


		m_indirectDrawSignature = MakeRef<IndirectSignature>();
		m_indirectDrawSignature->AddIndirectDrawArg();
		m_indirectDrawSignature->CreateSignature(nullptr, sizeof(IndirectArgs));

		
		Q_RENDER_INFO("FINISHED INITIALIZING SHADOW SYSTEM");
	}

	void DepthPrepassSystem::Render()
	{
		m_commandList->Reset();
		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "Depth Prepass System ms");

		//Shadow map pass
		m_framebuffer->Bind(m_commandList);
		{
			m_technique->Bind(m_commandList);

			QRender::RenderCommand::BindUniformBuffer(m_commandList, m_cameraDataUBO, 0);

			m_defaultMaterial->Bind(m_commandList, 5); //We're only using one material at the moment


			//If we have any batches
			if (m_renderData.renderBucket->batches.size() > 0 && false)
			{
				m_debugStats.objectCount = m_renderData.renderBucket->flatBatches.size();

				QRender::RenderCommand::BindStructuredBuffer(m_commandList, m_renderData.renderBucket->transformStructuredBuffer, 3);
				Ref<ModelAsset> lastMesh = nullptr;
				bool sameMesh = true;

				//if (!sameMesh)
				{
					QRender::RenderCommand::BindIndexBuffer(m_commandList, m_renderData.renderBucket->sceneCombinedIndexBuffer);
					QRender::RenderCommand::BindIndexBuffer(m_commandList, m_renderData.renderBucket->sceneCombinedVertexBuffer);

				}

				for (int i = 0; i < m_renderData.renderBucket->multibatches.size(); ++i)
				{
					auto& nextMultiBatch = m_renderData.renderBucket->multibatches[i];
					auto& nextInstanceDraw = m_renderData.renderBucket->batches[nextMultiBatch.first];

					//This is a check to see if there are any meshes that don't belong to the giant vertex buffer/index buffer
					//For the scene. If we don't have that, then we need to bind per object and rebind the scene buffer afterwards
					// 
					// 
					// if (object->GetShaderHash() != currentObject)
					//Compare if the next draw call has the same mesh
					//if (lastMesh->GetRef() == nextInstanceDraw.meshID)
					//{
					//
					//}

					m_debugStats.triangleCount += 12 * nextInstanceDraw.count;
					m_debugStats.drawCalls++;
					QRender::RenderCommand::IndirectDraw(m_commandList, m_indirectDrawSignature, nextMultiBatch.count, m_renderData.renderBucket->gpuClearIndirectCommandBuffer, nextMultiBatch.first, nullptr, 0);

				}
			}
		}
		m_framebuffer->Unbind(m_commandList);

		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void DepthPrepassSystem::Update()
	{
	}

	void DepthPrepassSystem::SetUpData()
	{

	}

	void DepthPrepassSystem::Dispatch()
	{

	}

	void DepthPrepassSystem::Bind()
	{

	}

	void DepthPrepassSystem::Unbind()
	{

	}

	void DepthPrepassSystem::DebugDraw()
	{
	}

	void DepthPrepassSystem::Resize(int width, int height)
	{
	}
}