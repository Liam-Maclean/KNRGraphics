#include <qlpch.h>
#include "graphics/primitives/plane.h"
#include "graphics/renderable_types.h"
#include "graphics/render_commands.h"
#include "forward_system.h"
#include "graphics/platform/directx/directx_indirect_signature.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_pipeline.h"
#include "graphics/platform/directx/directx_technique.h"
#include "assets/asset_database.h"
#include "assets/asset_loader.h"
#include "graphics/graphics_profiler.h"
#include "graphics/platform/directx/directx_command_buffer.h"

#include <limits>


namespace QRender
{
	ForwardSystem::ForwardSystem(RenderSystemManager& manager)
		: RenderSystem(manager)
	{

	}

	ForwardSystem::~ForwardSystem()
	{

	}

	void ForwardSystem::Init()
	{
		Q_RENDER_INFO("INITIALISING FORWARD RENDERING SYSTEM");

		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		m_debugGrid = MakeRef<QRender::Plane>();
		m_gridTransform = glm::mat4(1.0f);
		m_gridTransform = glm::scale(m_gridTransform, glm::vec3(100.0f, 100.0f, 100.0f));
		m_gridTransform = glm::translate(m_gridTransform, glm::vec3(-32.0f, 0.0f, -32.0f));
		m_gridTransform = glm::transpose(m_gridTransform);

		/* setup framebuffers */
		QRender::FramebufferSpecification fbSpec;
		fbSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::RGBA8,	//ALbedo
			QRender::FramebufferTextureFormat::RGBA8,	//Normal
			QRender::FramebufferTextureFormat::RGBA8,	//uv + roughness and metallic
			QRender::FramebufferTextureFormat::RG16,	//velocity buffer (TAA)
			QRender::FramebufferTextureFormat::Depth	//depth
		};
		fbSpec.Width = m_manager.GetRenderWidth();
		fbSpec.Height = m_manager.GetRenderHeight();

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = _countof(DirectXVertexInputLayouts::kVertexInputStaticMesh);
		inputLayoutDesc.pInputElementDescs = DirectXVertexInputLayouts::kVertexInputStaticMesh;

		//This is describing the techniques layout
		DirectXPipelineLayout pipelineLayout = {};
		pipelineLayout.AddConstantResourceView("camera matrices", 0, ShaderVisibility::all); //0
		pipelineLayout.AddDescriptorTable("gbuffer", 0, 7, 0, DescriptorTableType::srv, ShaderVisibility::all); // 1
		pipelineLayout.AddDescriptorTable("Bindless Textures", 8, -1, 1, DescriptorTableType::srv, ShaderVisibility::all); // 2
		pipelineLayout.AddShaderResourceView("Model Transforms", 7, ShaderVisibility::all); //3 
		pipelineLayout.AddConstantResourceView("Skinned matrices data", 1, ShaderVisibility::all);// 4
		pipelineLayout.AddConstantResourceView("Material data", 2, ShaderVisibility::all);// 5
		pipelineLayout.AddConstantResourceView("Directional Light", 3, ShaderVisibility::all); // 6
		pipelineLayout.AddConstantResourceView("debug shader values", 4, ShaderVisibility::all);// 7
		pipelineLayout.AddSampler("Point Sampler", 0, SamplerType::min_mag_mip_linear);
		pipelineLayout.AddSampler("Anisotropic Sampler", 1, SamplerType::anisotropic);
		pipelineLayout.CreateRootSignature();

		m_vertShader = QRender::Shader::Create("assets/shaders/StaticMesh/static_opaque_vert.qvert");
		m_fragShader = QRender::Shader::Create("assets/shaders/Deferred/deferred_forward_frag.qfrag");

		D3D12_SHADER_BYTECODE* vertBytecode = static_cast<D3D12_SHADER_BYTECODE*>(m_vertShader->GetShaderBytecode());
		D3D12_SHADER_BYTECODE* fragBytecode = static_cast<D3D12_SHADER_BYTECODE*>(m_fragShader->GetShaderBytecode());
		//This is the whole collection of info we need to create a new technique
		PipelineCreateInfo forwardPipelineCreateInfo = {};
		forwardPipelineCreateInfo.vertShader = *vertBytecode; //Shaders would be added to the asset database instead of being made manually
		forwardPipelineCreateInfo.pixelShader = *fragBytecode; //Shaders are just bytedata, so can be multithreaded on start up (COMPILING SHADERS OMEGALUL)
		forwardPipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM); //Albedo (XYZ), Transparency (w)
		forwardPipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM); //Normals (XYZ), roughness (W)
		forwardPipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM); //Emissive (XYZ), metallic (W)
		forwardPipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R16G16_FLOAT);	//Velocity buffer (XY)
		forwardPipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		forwardPipelineCreateInfo.pipelineLayout = pipelineLayout;
		forwardPipelineCreateInfo.vertexInputLayout = inputLayoutDesc; //We need a place for input layouts (per model perhaps?)
		forwardPipelineCreateInfo.wireframeModeEnabled = false;
		forwardPipelineCreateInfo.depthTestEnabled = true;
		forwardPipelineCreateInfo.cullMode = CullingMode::backface;

		m_technique = QRender::Technique::Create(forwardPipelineCreateInfo); //This is our final technique. We use this to bind the effect when we draw
																			 //If we want different shader permutations, we can easily cache and remake by changing the shaders
		
		m_indirectDrawSignature = MakeRef<IndirectSignature>();
		m_indirectDrawSignature->AddIndirectDrawArg();
		m_indirectDrawSignature->CreateSignature(nullptr, sizeof(IndirectArgs));
		//m_indirectDrawSignature->CreateSignature(m_technique, sizeof(indirectStruct));


		forwardPipelineCreateInfo.wireframeModeEnabled = true;
		m_wireframeTechnique = QRender::Technique::Create(forwardPipelineCreateInfo);
		
		m_framebuffer = QRender::Framebuffer::Create(fbSpec);
		QRender::BufferDescriptor cameraBufferDesc = {};
		cameraBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		cameraBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		cameraBufferDesc.size = sizeof(CameraDeferredData);
		cameraBufferDesc.debugName = L"Forward Camera UBO";
		m_cameraDataUBO = QRender::Buffer::Create(cameraBufferDesc);

		QRender::BufferDescriptor frameBufferDesc = {};
		frameBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		frameBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		frameBufferDesc.size = sizeof(FrameSettings);
		frameBufferDesc.debugName = L"Frame Camera buffer";
		m_frameSettingsDataUBO = QRender::Buffer::Create(frameBufferDesc);

		m_defaultMaterial = MakeRef<QRender::PBRMaterial>();
		m_defaultMaterial->CreateShaderResources();

		void* frameSettingsMapAddr = m_frameSettingsDataUBO->Map();
		memcpy(frameSettingsMapAddr, static_cast<void*>(&m_renderData.frameSettings), sizeof(FrameSettings));
		m_frameSettingsDataUBO->UnMap();

		Q_RENDER_INFO("FINISHED INITIALIZING FORWARD RENDERING SYSTEM");
	}

	void ForwardSystem::Render()
	{
		//Reset debug info
		m_debugStats.drawCalls = 0;
		m_debugStats.objectCount = 0;
		m_debugStats.triangleCount = 0;

		//Reset command queue, start recording
		m_commandList->Reset();

		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "Forward System ms");
		


		//G-Buffer MRT pass
		m_framebuffer->Bind(m_commandList);
		{
			
			//Take this out for now till I figure a more customisable way of doing these 
			if (m_wireframeMode)
			{
				m_wireframeTechnique->Bind(m_commandList);
			}
			else
			{
				m_technique->Bind(m_commandList);
			}
			
			QRender::RenderCommand::BindUniformBuffer(m_commandList, m_cameraDataUBO, 0);
			
			m_defaultMaterial->Bind(m_commandList, 5); //We're only using one material at the moment
	

			//If we have any batches
			if (m_renderData.renderBucket->batches.size() > 0 && false)
			{
				m_debugStats.objectCount = m_renderData.renderBucket->flatBatches.size();

				//m_renderData.renderBucket->transformStructuredBuffer->Bind(m_commandList, 3);
				Ref<ModelAsset> lastMesh = nullptr;
				bool sameMesh = true;
				
				//if (!sameMesh)
				{
					//m_renderData.renderBucket->sceneCombinedIndexBuffer->Bind(m_commandList);
					//m_renderData.renderBucket->sceneCombinedVertexBuffer->Bind(m_commandList);
					
				}
				
				for(int i = 0; i < m_renderData.renderBucket->multibatches.size(); ++i)
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
					QRender::RenderCommand::IndirectDraw(m_commandList, m_indirectDrawSignature, nextMultiBatch.count, m_renderData.renderBucket->gpuClearIndirectCommandBuffer, nextMultiBatch.first,nullptr, 0);
					
				}
			}
			//QRender::RenderCommand::IndirectDraw(m_indirectDrawSignature);
			//for (m_renderData.renderBucket->)
			//{
			//
			//}
			//
			//std::map< entt::entity, InstancedRenderable>::iterator it;
			//for (it = m_renderData.renderables->begin(); it != m_renderData.renderables->end(); it++)
			//{
			//	Ref<StaticMeshAsset> asset = static_pointer_cast<StaticMeshAsset>(it->second.mesh);
			//	QRender::RenderCommand::SetGraphicsRootConstants(7, 16, &it->second.transform, 0);
			//	DrawStatic(*asset->GetRef());
			//}
			//
			////Debug grid
			//if (m_debugGridEnabled)
			//{
			//	m_wireframeTechnique->Bind();
			//	m_cameraDataUBO->Bind(0);
			//	m_defaultMaterial->Bind(4); //We're only using one material at the moment
			//	//QRender::RenderCommand::SetGraphicsRootConstants(7, 16, &m_gridTransform, 0);
			//	m_debugGrid->Draw();
			//}

		}
		m_framebuffer->Unbind(m_commandList);

		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void ForwardSystem::Update()
	{
	}

	void ForwardSystem::SetUpData()
	{
	}
	void ForwardSystem::Dispatch()
	{
	}
	void ForwardSystem::Bind()
	{
	}
	void ForwardSystem::Unbind()
	{
	}

	void ForwardSystem::Resize(int width, int height)
	{
		m_framebuffer->Resize(width, height);
	}

	void ForwardSystem::DebugDraw()
	{
		ImGui::Begin("Forward Render System Debug", &m_debugWindowEnabled);
		ImGui::Separator();
		ImGui::Text("Objects: %d", m_debugStats.objectCount);
		ImGui::Text("Drawcalls: %d", m_debugStats.drawCalls);
		ImGui::Text("Triangles: %d", m_debugStats.triangleCount);
		ImGui::End();
	}

	void ForwardSystem::UpdateCameraData(CameraDeferredData inCameraData)
	{
		m_renderData.cameraSettings = inCameraData;
		void* cameraBufferMapAddr = m_cameraDataUBO->Map();
		memcpy(cameraBufferMapAddr, static_cast<void*>(&m_renderData.cameraSettings), sizeof(CameraDeferredData));
		m_cameraDataUBO->UnMap();
	}

	void ForwardSystem::UpdateShaderData(RenderData renderData)
	{
		m_renderData = renderData;
		void* frameSettingsMapAddr = m_frameSettingsDataUBO->Map();
		memcpy(frameSettingsMapAddr, static_cast<void*>(&m_renderData.frameSettings), sizeof(FrameSettings));
		m_frameSettingsDataUBO->UnMap();

		void* cameraBufferMapAddr = m_cameraDataUBO->Map();
		memcpy(cameraBufferMapAddr, static_cast<void*>(&m_renderData.cameraSettings), sizeof(CameraDeferredData));
		m_cameraDataUBO->UnMap();
	}

	void ForwardSystem::UpdateFrameSettings(FrameSettings inFrameSettings)
	{
		m_renderData.frameSettings = inFrameSettings;
		void* frameSettingsMapAddr = m_frameSettingsDataUBO->Map();
		memcpy(frameSettingsMapAddr, static_cast<void*>(&m_renderData.frameSettings), sizeof(FrameSettings));
		m_frameSettingsDataUBO->UnMap();
	}

	void ForwardSystem::SetRenderBucket(RenderBucket* inRenderBucket)
	{
		m_renderData.renderBucket = inRenderBucket;
	}

	//TODO Liam - To remove this
	void ForwardSystem::DrawNode(QCore::Node* node)
	{
		if (node->mesh)
		{
			//node->mesh->uniformBuffer.nodeMatrixBuffer->Bind(m_commandList, 3);
			for (int j = 0; j < node->mesh->submeshes.size(); ++j)
			{
			
				node->mesh->submeshes[j]->subMeshMaterial.Bind(m_commandList, 4);
				m_commandList->Get()->DrawIndexedInstanced(node->mesh->submeshes[j]->indexCount, 1, node->mesh->submeshes[j]->firstIndex, 0, 0);
			}
		}

		for (auto& child : node->children)
		{
			DrawNode(child);
		}
	}

	//TODO Liam - To remove this
	void ForwardSystem::Draw(QCore::GLTFStructure& gltfModel)
	{
		gltfModel.m_vertexBuffer->Bind(m_commandList);
		gltfModel.m_indexBuffer->Bind(m_commandList);

		for (auto& node : gltfModel.nodes)
		{
			DrawNode(node);
		}
	}

	void ForwardSystem::DrawStatic(const QRender::StaticMesh& staticMesh)
	{
		//staticMesh.indexBuffer->Bind(m_commandList);
		//staticMesh.vertexBuffer->Bind(m_commandList);
		m_commandList->Get()->DrawIndexedInstanced(staticMesh.indexCount, 1, 0, 0, 0);

	}

	void ForwardSystem::SetRenderables(std::map<entt::entity, InstancedRenderable>& renderables)
	{
		m_renderData.renderables = &renderables;
	}
}