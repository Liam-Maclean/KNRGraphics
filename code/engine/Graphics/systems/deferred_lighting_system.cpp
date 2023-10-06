#include <qlpch.h>
#include "deferred_lighting_system.h"
#include "graphics/render_commands.h"
#include "graphics/systems/shadow_system.h"
#include "graphics/systems/forward_system.h"
#include "graphics/platform/directx/directx_technique.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_pipeline.h"
#include "graphics/platform/directx/directx_pipeline_layout.h"
#include "graphics/graphics_profiler.h"

namespace QRender
{
	LightingSystem::LightingSystem(RenderSystemManager& manager)
		: RenderQuadSystem(manager)
	{

	}

	LightingSystem::~LightingSystem()
	{

	}

	void LightingSystem::Init()
	{
		Q_RENDER_INFO("INITIALISING LIGHTING SYSTEM");

		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		QRender::FramebufferSpecification deferredCompositionSpec;
		deferredCompositionSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::RGBA8,
		};
		deferredCompositionSpec.Width = m_manager.GetRenderWidth();
		deferredCompositionSpec.Height = m_manager.GetRenderHeight();
		m_framebuffer = QRender::Framebuffer::Create(deferredCompositionSpec);

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = _countof(DirectXVertexInputLayouts::kVertexInputQuad);
		inputLayoutDesc.pInputElementDescs = DirectXVertexInputLayouts::kVertexInputQuad;

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

		m_vertShader = QRender::Shader::Create("assets/shaders/Quad/quad_vert.qvert");
		m_fragShader = QRender::Shader::Create("assets/shaders/Deferred/deferred_lighting_frag.qfrag");

		D3D12_SHADER_BYTECODE* vertBytecode = static_cast<D3D12_SHADER_BYTECODE*>(m_vertShader->GetShaderBytecode());
		D3D12_SHADER_BYTECODE* fragBytecode = static_cast<D3D12_SHADER_BYTECODE*>(m_fragShader->GetShaderBytecode());

		//This is the whole collection of info we need to create a new technique
		PipelineCreateInfo lightingPipelineCreateInfo = {};
		lightingPipelineCreateInfo.vertShader = *vertBytecode; //Shaders would be added to the asset database instead of being made manually
		lightingPipelineCreateInfo.pixelShader = *fragBytecode; //Shaders are just bytedata, so can be multithreaded on start up (COMPILING SHADERS OMEGALUL)
		lightingPipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM); //What render targets are we outputting to? Use the framebuffers descriptors perhaps?
		lightingPipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		lightingPipelineCreateInfo.pipelineLayout = pipelineLayout;
		lightingPipelineCreateInfo.vertexInputLayout = inputLayoutDesc; //We need a place for input layouts (per model perhaps?)
		lightingPipelineCreateInfo.wireframeModeEnabled = false;
		lightingPipelineCreateInfo.cullMode = CullingMode::backface;
	
		m_technique = QRender::Technique::Create(lightingPipelineCreateInfo);


		m_shadowSystem = m_manager.GetRenderSystem<QRender::ShadowSystem>();
		m_forwardSystem = m_manager.GetRenderSystem<QRender::ForwardSystem>();

		m_gBufferAlbedo = m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(0);
		m_gBufferNormal = m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(1);
		m_gBufferEmissive = m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(2);
		m_gBufferDepth = m_forwardSystem->GetFramebuffer()->GetDepthTexture();
		m_shadowMap = m_shadowSystem->GetFramebuffer()->GetDepthTexture();

		
		//LIAM - FIX WHEN WE COME BACK TO THIS, WE SHOULDN'T BE LOADING ASSETS DIRECTLY ANYMORE
		//m_brdfLUT = QRender::Texture2D::Create("assets/textures/ibl_brdf_lut.png");
		//m_brdfLUT->Upload();
		//m_skybox = QRender::Texture2D::Create(true);

		QRender::BufferDescriptor directionalLightBufferDesc = {};
		directionalLightBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		directionalLightBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		directionalLightBufferDesc.size = sizeof(DirectionalLightData);
		directionalLightBufferDesc.debugName = L"Directional Light UBO";
		m_directionalLightDataUBO = QRender::Buffer::Create(directionalLightBufferDesc);

		QRender::BufferDescriptor cameraBufferDesc = {};
		cameraBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		cameraBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		cameraBufferDesc.size = sizeof(CameraDeferredData);
		cameraBufferDesc.debugName = L"Camera UBO";
		m_cameraDataUBO = QRender::Buffer::Create(cameraBufferDesc);

		QRender::BufferDescriptor shaderDebugBufferDesc = {};
		shaderDebugBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		shaderDebugBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		shaderDebugBufferDesc.size = sizeof(ShaderDebugValues);
		shaderDebugBufferDesc.debugName = L"Camera UBO";
		m_shaderDebugValuesDataUBO = QRender::Buffer::Create(shaderDebugBufferDesc);

		m_renderData.shaderDebugValuesData = {};
		m_renderData.shaderDebugValuesData.HDREnabled = 0;
		m_renderData.shaderDebugValuesData.GammaCorrectionEnabled = 0;
		m_renderData.shaderDebugValuesData.viewMode = 0;
		m_renderData.shaderDebugValuesData.iblEnabled = 1;
		m_renderData.shaderDebugValuesData.shadowsEnabled = 1;
		m_renderData.shaderDebugValuesData.skyboxEnabled = 1;

		//Lets just have some dummy values for the debug stuff
		void* shaderDebugMapAddr = m_shaderDebugValuesDataUBO->Map();
		memcpy(shaderDebugMapAddr, static_cast<void*>(&m_renderData.shaderDebugValuesData), sizeof(ShaderDebugValues));
		m_shaderDebugValuesDataUBO->UnMap();

		Q_RENDER_INFO("FINISHED INITIALIZING LIGHTING SYSTEM");

	}

	void LightingSystem::Render()
	{
		m_commandList->Reset();
		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "Lighting System ms");
		m_framebuffer->Bind(m_commandList);
		{
			//Set up the pipeline and bind the technique
			m_technique->Bind(m_commandList);

			//Bind Lighting values
			QRender::RenderCommand::BindUniformBuffer(m_commandList, m_cameraDataUBO, 0);

			QRender::RenderCommand::BindUniformBuffer(m_commandList, m_directionalLightDataUBO, 5);

			QRender::RenderCommand::BindUniformBuffer(m_commandList, m_shaderDebugValuesDataUBO, 6);

			ScreenQuadObject.Draw(m_commandList);
		}
		m_framebuffer->Unbind(m_commandList);
		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void LightingSystem::Update()
	{

	}

	void LightingSystem::SetUpData()
	{

	}

	void LightingSystem::Dispatch()
	{

	}

	void LightingSystem::Bind()
	{

	}

	void LightingSystem::Unbind()
	{

	}

	void LightingSystem::DebugDraw()
	{
	}

	void LightingSystem::Resize(int width, int height)
	{
		m_framebuffer->Resize(width, height);


		//Reassign the pointers to the new resized ones
		m_gBufferAlbedo = m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(0);
		m_gBufferNormal = m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(1);
		m_gBufferEmissive = m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(2);
		m_gBufferDepth = m_forwardSystem->GetFramebuffer()->GetDepthTexture();
		m_shadowMap = m_shadowSystem->GetFramebuffer()->GetDepthTexture();
	}

	void LightingSystem::UpdateShaderData(RenderData renderData)
	{
		m_renderData = renderData;

		void* cameraBufferMapAddr = m_cameraDataUBO->Map();
		memcpy(cameraBufferMapAddr, static_cast<void*>(&m_cameraData), sizeof(CameraDeferredData));
		m_cameraDataUBO->UnMap();

		void* directionalLightMapAddr = m_directionalLightDataUBO->Map();
		memcpy(directionalLightMapAddr, static_cast<void*>(&m_directionalLightData), sizeof(DirectionalLightData));
		m_directionalLightDataUBO->UnMap();

		void* shaderDebugMapAddr = m_shaderDebugValuesDataUBO->Map();
		memcpy(shaderDebugMapAddr, static_cast<void*>(&m_shaderDebugValuesData), sizeof(ShaderDebugValues));
		m_shaderDebugValuesDataUBO->UnMap();
	}

	void LightingSystem::UpdateCameraData(CameraDeferredData cameraDeferredData)
	{
		m_renderData.cameraData = cameraDeferredData;
		void* cameraBufferMapAddr = m_cameraDataUBO->Map();
		memcpy(cameraBufferMapAddr, static_cast<void*>(&m_renderData.cameraData), sizeof(CameraDeferredData));
		m_cameraDataUBO->UnMap();
	}

	void LightingSystem::UpdateLightData(DirectionalLightData directionalLightData)
	{
		m_renderData.directionalLightData = directionalLightData;
		void* directionalLightMapAddr = m_directionalLightDataUBO->Map();
		memcpy(directionalLightMapAddr, static_cast<void*>(&m_renderData.directionalLightData), sizeof(DirectionalLightData));
		m_directionalLightDataUBO->UnMap();
	}
	
	void LightingSystem::UpdateDebugSettings(ShaderDebugValues shaderDebugValues)
	{
		m_renderData.shaderDebugValuesData = shaderDebugValues;
		void* shaderDebugMapAddr = m_shaderDebugValuesDataUBO->Map();
		memcpy(shaderDebugMapAddr, static_cast<void*>(&m_renderData.shaderDebugValuesData), sizeof(ShaderDebugValues));
		m_shaderDebugValuesDataUBO->UnMap();
	}
}