#include <qlpch.h>
#include "taa_system.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/systems/forward_system.h"
#include "graphics/render_commands.h"

namespace QRender
{

	TAASystem::TAASystem(RenderSystemManager& manager)
		: RenderQuadSystem(manager)
	{

	}

	TAASystem::~TAASystem()
	{

	}

	void TAASystem::Init()
	{
		Q_RENDER_INFO("INITIALISING TAA SYSTEM");

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
		m_forwardSystem = m_manager.GetRenderSystem<QRender::ForwardSystem>();
		m_lightingSystem = m_manager.GetRenderSystem<QRender::LightingSystem>();
		m_compositionTexture = m_lightingSystem->GetFramebuffer()->GetFramebufferTexture(0);
		m_depthTexture = m_forwardSystem->GetFramebuffer()->GetDepthTexture();
		m_velocityTexture = m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(3);
		//We need the post textures from the blit later, how do we get those O_O

		//Create hollow versions of the previous frame (Frame - 1)
		QRender::FramebufferTextureSpecification RGB8TexSpec(QRender::FramebufferTextureFormat::RGBA8);
		QRender::FramebufferTextureSpecification DepthTexSpec(QRender::FramebufferTextureFormat::DEPTH24STENCIL8);

		QRender::TextureDescriptor textureDesc = {};
		textureDesc.textureType == TextureType::FRAMEBUFFER;
		textureDesc.textureUsage == TextureUsage::RENDERTARGET;
		textureDesc.textureFramebuffer.framebufferTextureSpec = &RGB8TexSpec;
		textureDesc.textureFramebuffer.framebufferSpec = &fbSpec;

		m_previousCompositionTexture = QRender::Texture2D::Create(textureDesc);

		textureDesc.textureFramebuffer.framebufferTextureSpec = &DepthTexSpec; //We can reuse most of the descriptor instead of making a new one
		m_previousDepthTexture = QRender::Texture2D::Create(textureDesc);

		QRender::BufferDescriptor taaBufferDesc = {};
		taaBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		taaBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		taaBufferDesc.size = sizeof(TAASettings);
		taaBufferDesc.debugName = L"TAA uniform buffer";
		m_taaSettingsDataUBO = QRender::Buffer::Create(taaBufferDesc);

		QRender::BufferDescriptor frameSettingsBufferDesc = {};
		frameSettingsBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		frameSettingsBufferDesc.bufferType = QRender::BufferUsageType::UNIFORM;
		frameSettingsBufferDesc.size = sizeof(FrameSettings);
		frameSettingsBufferDesc.debugName = L"Frame Settings uniform buffer";
		m_frameSettingsDataUBO = QRender::Buffer::Create(frameSettingsBufferDesc);


		m_frameSettings = {};
		m_frameSettings.deltaTime = 0;
		m_frameSettings.elapsedTime = 0;
		m_frameSettings.resolution = { 1920, 1080 };
		m_frameSettings.totalFrames = 0;
	
		m_renderData.taaSettings.feedbackFactor = 0;
		m_renderData.taaSettings.maxDepthFalloff = 0;
		m_renderData.taaSettings.velocityScale = 0;
		m_renderData.taaSettings.taaEnabled = 1;

		void* taaBufferMapAddr = m_taaSettingsDataUBO->Map();	
		memcpy(taaBufferMapAddr, static_cast<void*>(&m_renderData.taaSettings), sizeof(TAASettings));
		m_taaSettingsDataUBO->UnMap();
	}

	void TAASystem::Render()
	{
		m_commandList->Reset();

		m_framebuffer->Bind(m_commandList);
		{
			//Set up the pipeline and bind the technique
			m_shader->Bind();

			//UBOs
			QRender::RenderCommand::BindUniformBuffer(m_commandList, m_taaSettingsDataUBO, 0);
			QRender::RenderCommand::BindUniformBuffer(m_commandList, m_frameSettingsDataUBO, 1);

			ScreenQuadObject.Draw(m_commandList);
		}
		m_framebuffer->Unbind(m_commandList);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();

		QRender::RenderCommand::BlitToTexture(m_compositionTexture, m_previousCompositionTexture);
		QRender::RenderCommand::BlitToTexture(m_depthTexture, m_previousDepthTexture);
	}

	void TAASystem::Update()
	{
	}

	void TAASystem::UpdateTAASettings(TAASettings inTaaSettings)
	{
		m_renderData.taaSettings = inTaaSettings;
		void* taaBufferMapAddr = m_taaSettingsDataUBO->Map();
		memcpy(taaBufferMapAddr, static_cast<void*>(&m_renderData.taaSettings), sizeof(TAASettings));
		m_taaSettingsDataUBO->UnMap();
	}

	void TAASystem::UpdateFrameSettings(FrameSettings inFrameSettings)
	{
		m_renderData.frameSettings = inFrameSettings;

		void* frameSettingsMapAddr = m_frameSettingsDataUBO->Map();
		memcpy(frameSettingsMapAddr, static_cast<void*>(&m_renderData.frameSettings), sizeof(FrameSettings));
		m_frameSettingsDataUBO->UnMap();
	}

	void TAASystem::Resize(int width, int height)
	{
	}

	void TAASystem::CalculateJitter()
	{
		//index is the total frames modulo sample count
		int index = m_renderData.frameSettings.totalFrames % 128;

		//Calculate Halton Jitter for TAA
		float haltonX = QMaths::Halton(index + 1, 2);
		float haltonY = QMaths::Halton(index + 1, 3);
		m_jitter = Vector2f(haltonX, haltonY);
		m_projJitter.x = 2.0f * m_jitter.x / m_manager.GetViewportWidth();
		m_projJitter.y = -2.0f * m_jitter.y / m_manager.GetViewportHeight();
	}

	void TAASystem::SetUpData()
	{

	}

	void TAASystem::Dispatch()
	{

	}

	void TAASystem::Bind()
	{

	}

	void TAASystem::Unbind()
	{

	}

	void TAASystem::DebugDraw()
	{
	}



}