#include <qlpch.h>
#include "forward_plus_accumulation_system.h"
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

	ForwardPlusAccumulationSystem::ForwardPlusAccumulationSystem(RenderSystemManager& manager)
		: RenderSystem(manager)
	{

	}

	ForwardPlusAccumulationSystem::~ForwardPlusAccumulationSystem()
	{

	}

	void ForwardPlusAccumulationSystem::Init()
	{
		Q_RENDER_INFO("INITIALISING FORWARD PLUS ACCUMULATION SYSTEM");

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

		//Preload some shaders (temp)
		QRender::Shader::Create("assets/shaders/StaticMesh/forward_simple_vert.qvert");
		QRender::Shader::Create("assets/shaders/StaticMesh/forward_simple_frag.qfrag");
		
		//Buffer data
		QRender::BufferDescriptor cameraBufferDesc = {};
		cameraBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		cameraBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		cameraBufferDesc.size = sizeof(CameraDeferredData);
		cameraBufferDesc.debugName = L"Forward Camera UBO";
		m_cameraDataUBO = QRender::Buffer::Create(cameraBufferDesc);

		QRender::BufferDescriptor directionalLightBufferDesc = {};
		directionalLightBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		directionalLightBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		directionalLightBufferDesc.size = sizeof(InstancedDirectionalLight);
		directionalLightBufferDesc.debugName = L"Directional Light UBOs";
		m_directionalLightBuffer = QRender::Buffer::Create(directionalLightBufferDesc);

		InstancedDirectionalLight instancedDirectionalLight = {};
		instancedDirectionalLight.color = Vector4f(1.0f, 1.0f, 1.0f, 20.0f);
		instancedDirectionalLight.direction = Vector4f(0.0f, 1.0f, -1.0f, 1.0f);
		instancedDirectionalLight.lightPos = Vector4f((instancedDirectionalLight.direction.x * 40), (instancedDirectionalLight.direction.y * 40), (instancedDirectionalLight.direction.z * 40), 1.0f);
		
		void* mapAddr = m_directionalLightBuffer->Map();
		memcpy(mapAddr, &instancedDirectionalLight, sizeof(InstancedDirectionalLight));
		m_directionalLightBuffer->UnMap();
		//QRender::DirectionalLightData directionalLightData = {};
		//directionalLightData.color = glm::vec4(1.0f, 1.0f, 1.0f, 20.0f);
		//directionalLightData.lightDir = glm::vec4(0.0f, 1.0f, -1.0f, 1.0f);
		//directionalLightData.lightPos = Vector4f((directionalLightData.lightDir.x * 40), (directionalLightData.lightDir.y * 40), (directionalLightData.lightDir.z * 40), 1.0f);


		QRender::BufferDescriptor pointLightBufferDesc = {};
		pointLightBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		pointLightBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		pointLightBufferDesc.size = sizeof(InstancedPointLight) * 24;
		pointLightBufferDesc.debugName = L"Point Light UBOs";
		m_pointLightBuffer = QRender::Buffer::Create(pointLightBufferDesc);

		QRender::BufferDescriptor spotLightBufferDesc = {};
		spotLightBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		spotLightBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		spotLightBufferDesc.size = sizeof(InstancedSpotLight) * 24;
		spotLightBufferDesc.debugName = L"Spot Light UBOs";
		m_spotLightBuffer = QRender::Buffer::Create(spotLightBufferDesc);

		//Indirect signature (we should change this eventually)
		m_indirectDrawSignature = MakeRef<IndirectSignature>();
		m_indirectDrawSignature->AddIndirectDrawArg();
		m_indirectDrawSignature->CreateSignature(nullptr, sizeof(IndirectArgs));

		Q_RENDER_INFO("FINISHED INITIALIZING FORWARD PLUS ACCUMULATION SYSTEM");
	}

	void ForwardPlusAccumulationSystem::Render()
	{
		m_commandList->Reset();
		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "Forward+ Accumulation System ms");

		//Shadow map pass
		m_framebuffer->Bind(m_commandList);
		{
			//If we have any batches
			if (m_renderData.renderBucket->batches.size() > 0)
			{
				m_debugStats.objectCount = m_renderData.renderBucket->flatBatches.size();
	
				//Bind the scene sized vertex and index buffer
				QRender::RenderCommand::BindVertexBuffer(m_commandList, m_renderData.renderBucket->sceneCombinedVertexBuffer);
				QRender::RenderCommand::BindIndexBuffer(m_commandList, m_renderData.renderBucket->sceneCombinedIndexBuffer);
				
				uint32_t previousTechniqueId = 0;

				for (int i = 0; i < m_renderData.renderBucket->multibatches.size(); ++i)
				{
					auto& nextMultiBatch = m_renderData.renderBucket->multibatches[i];
					auto& nextInstanceDraw = m_renderData.renderBucket->batches[nextMultiBatch.first];

					//Bind shader if it changes
					QRender::RenderCommand::BindPipeline(m_commandList, nextInstanceDraw.technique);

					//Bind uniforms
					//Temporary, to move to render command
					nextInstanceDraw.material->Bind(m_commandList, 5);
					QRender::RenderCommand::BindStructuredBuffer(m_commandList, m_renderData.renderBucket->transformStructuredBuffer, 3);
					QRender::RenderCommand::BindUniformBuffer(m_commandList, m_directionalLightBuffer, 5);
					QRender::RenderCommand::BindUniformBuffer(m_commandList, m_cameraDataUBO, 0);
				
					m_debugStats.drawCalls++;
					QRender::RenderCommand::IndirectDraw(m_commandList, m_indirectDrawSignature, nextMultiBatch.count, m_renderData.renderBucket->gpuClearIndirectCommandBuffer, nextMultiBatch.first, nullptr, 0);
					
					//Hold the previous technique incase it changes and we need to rebind
					previousTechniqueId = nextInstanceDraw.techniqueId;

				}
			}
		}
		m_framebuffer->Unbind(m_commandList);
		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void ForwardPlusAccumulationSystem::Update()
	{
	}

	void ForwardPlusAccumulationSystem::SetUpData()
	{

	}

	void ForwardPlusAccumulationSystem::Dispatch()
	{

	}

	void ForwardPlusAccumulationSystem::Bind()
	{

	}

	void ForwardPlusAccumulationSystem::Unbind()
	{

	}

	void ForwardPlusAccumulationSystem::DebugDraw()
	{
	}

	void ForwardPlusAccumulationSystem::Resize(int width, int height)
	{
	}

	void ForwardPlusAccumulationSystem::UpdateCameraData(CameraDeferredData inCameraData)
	{
		m_renderData.cameraSettings = inCameraData;
		void* mapAddr = m_cameraDataUBO->Map();
		memcpy(mapAddr, static_cast<void*>(&m_renderData.cameraSettings), sizeof(CameraDeferredData));
		m_cameraDataUBO->UnMap();
	}

	void ForwardPlusAccumulationSystem::SetRenderBucket(RenderBucket* inRenderBucket)
	{
		m_renderData.renderBucket = inRenderBucket;
	}
}