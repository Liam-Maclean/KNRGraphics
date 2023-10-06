#include <qlpch.h>
#include "fxaa_system.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_pipeline.h"
#include "graphics/platform/directx/directx_pipeline_layout.h"
#include "graphics/systems/taa_system.h"
#include "graphics/render_commands.h"


namespace QRender
{

	FXAASystem::FXAASystem(RenderSystemManager& manager)
		: RenderQuadSystem(manager)
	{

	}

	FXAASystem::~FXAASystem()
	{

	}

	void FXAASystem::Init()
	{
		Q_RENDER_INFO("INITIALISING FXAA SYSTEM");

		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		QRender::FramebufferSpecification fbSpec;
		fbSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::RGBA8,
		};
		fbSpec.Width = 1920;
		fbSpec.Height = 1080;
		m_framebuffer = QRender::Framebuffer::Create(fbSpec);

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = _countof(DirectXVertexInputLayouts::kVertexInputQuad);
		inputLayoutDesc.pInputElementDescs = DirectXVertexInputLayouts::kVertexInputQuad;

		//This is describing the techniques layout
		DirectXPipelineLayout pipelineLayout = {};
		pipelineLayout.AddDescriptorTable("Input Image", 0, 1,0, DescriptorTableType::srv, ShaderVisibility::all);
		pipelineLayout.AddSampler("Point Sampler", 0, SamplerType::min_mag_mip_linear);

		////This is the whole collection of info we need to create a new technique
		//PipelineCreateInfo lightingPipelineCreateInfo = {};
		//lightingPipelineCreateInfo.vertShader = GetAssetManager()->RequestAsset("PathToVertShader"); //Shaders would be added to the asset database instead of being made manually
		//lightingPipelineCreateInfo.pixelShader = GetAssetManager()->RequestAsset("PathToPixelShader"); //Shaders are just bytedata, so can be multithreaded on start up (COMPILING SHADERS OMEGALUL)
		//lightingPipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM); //What render targets are we outputting to? Use the framebuffers descriptors perhaps?
		//lightingPipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//lightingPipelineCreateInfo.pipelineLayout = &pipelineLayout;
		//lightingPipelineCreateInfo.vertexInputLayout = inputLayoutDesc; //We need a place for input layouts (per model perhaps?)
		//lightingPipelineCreateInfo.wireframeModeEnabled = false;
		//lightingPipelineCreateInfo.depthTestEnabled = false;

		m_taaSystem = m_manager.GetRenderSystem<QRender::TAASystem>();
		m_taaResolveTexture = m_taaSystem->GetFramebuffer()->GetFramebufferTexture(0);

		uint64_t texId = m_framebuffer->GetColorAttachmentRendererID();
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = DirectXContext.GetImGuiHeap()->handleGPU(texId);
		QRender::RenderCommand::SetFinalRenderTextureId(reinterpret_cast<void*>(textureHandle.ptr));
	}

	void FXAASystem::Render()
	{
		m_commandList->Reset();

		//FXAA Anti aliasing
		m_framebuffer->Bind(m_commandList);
		{
			//Set up the pipeline and bind the technique
			m_shader->Bind();

			//Draw Quad
			ScreenQuadObject.Draw(m_commandList);

		}
		m_framebuffer->Unbind(m_commandList);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void FXAASystem::Update()
	{
	}

	void FXAASystem::SetUpData()
	{

	}

	void FXAASystem::Dispatch()
	{

	}

	void FXAASystem::Bind()
	{

	}

	void FXAASystem::Unbind()
	{

	}

	void FXAASystem::DebugDraw()
	{
	}

	void FXAASystem::Resize(int width, int height)
	{

	}
}