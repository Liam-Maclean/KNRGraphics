#include <qlpch.h>
#include "ffx_fsr2_system.h"
#include "graphics/platform/directx/directx_texture.h"
#include "graphics/systems/deferred_lighting_system.h"
#include "graphics/systems/forward_system.h"
#include "graphics/systems/deferred_lighting_system.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/graphics_profiler.h"
#include "graphics/render_commands.h"
#include "ffx-fsr2-api/ffx_fsr2.h"
#include "ffx-fsr2-api/dx12/ffx_fsr2_dx12.h"
#include "graphics/platform/directx/d3dx12.h"

#include "application/application.h"
namespace QRender
{
	FSR2System::FSR2System(RenderSystemManager& manager)
		: RenderQuadSystem(manager)
	{

	}

	FSR2System::~FSR2System()
	{

	}

	float FSR2System::GetFSRUpscaleRatio(FSRUpscaleMode upscaleMode)
	{
		float upscaleRatio;
		switch (upscaleMode)
		{
		case FSRUpscaleMode::NONE:
			upscaleRatio = 1.0f;
			break;
		case FSRUpscaleMode::ULTRA_QUALITY:
			upscaleRatio = 1.3f;
			break;
		case FSRUpscaleMode::QUALITY:
			upscaleRatio = 1.5f;
			break;
		case FSRUpscaleMode::BALANCE:
			upscaleRatio = 1.7f;
			break;
		case FSRUpscaleMode::PERFORMANCE:
			upscaleRatio = 2.0f;
			break;
		case FSRUpscaleMode::ULTRA_PERFORMANCE:
			upscaleRatio = 3.0f;
		}

		return upscaleRatio;
	}

	void FSR2System::Init()
	{
		Q_RENDER_INFO("INITIALISING FSR2 SYSTEM");
		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		m_frameCount = 0;
		QRender::FramebufferSpecification fbSpec;
		fbSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::RGBA8,
		};
		
		fbSpec.Width = m_manager.GetViewportWidth();
		fbSpec.Height = m_manager.GetViewportHeight();

		m_framebuffer = QRender::Framebuffer::Create(fbSpec);

		//Ultra Quality
		m_manager.SetUpscaleRatio(GetFSRUpscaleRatio(FSRUpscaleMode::ULTRA_QUALITY));

		
		m_forwardSystem = m_manager.GetRenderSystem<QRender::ForwardSystem>();
		m_lightingSystem = m_manager.GetRenderSystem<QRender::LightingSystem>();

		m_compositionTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_lightingSystem->GetFramebuffer()->GetFramebufferTexture(0));
		m_depthTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_forwardSystem->GetFramebuffer()->GetDepthTexture());
		m_velocityTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(3));
		m_outputTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_framebuffer->GetFramebufferTexture(0));

		m_fsrContext = MakeRef<FfxFsr2Context>();
		m_fsrContextDescription = MakeRef<FfxFsr2ContextDescription>();
		// Setup DX12 interface.
		const size_t scratchBufferSize = ffxFsr2GetScratchMemorySizeDX12();
		void* scratchBuffer = malloc(scratchBufferSize);
		FfxErrorCode errorCode = ffxFsr2GetInterfaceDX12(&m_fsrContextDescription->callbacks, DirectXContext.GetDevice(), scratchBuffer, scratchBufferSize);
		FFX_ASSERT(errorCode == FFX_OK);

		// Set up the context description.
		m_fsrContextDescription->device = ffxGetDeviceDX12(DirectXContext.GetDevice());
		m_fsrContextDescription->maxRenderSize.width = m_manager.GetRenderWidth();
		m_fsrContextDescription->maxRenderSize.height = m_manager.GetRenderHeight();
		m_fsrContextDescription->displaySize.width = m_manager.GetViewportWidth();
		m_fsrContextDescription->displaySize.height = m_manager.GetViewportHeight();
		m_fsrContextDescription->flags = 
			  FFX_FSR2_ENABLE_DEPTH_INVERTED
			| FFX_FSR2_ENABLE_AUTO_EXPOSURE;

		// Create the FSR2 context.
		errorCode = ffxFsr2ContextCreate(m_fsrContext.get(), m_fsrContextDescription.get());
		FFX_ASSERT(errorCode == FFX_OK);
		Q_RENDER_INFO("FINISHED INITIALIZING FSR2 SYSTEM");
	}

	void FSR2System::Render()
	{
		m_deltaTime = 0.085f;
		
		m_commandList->Reset();

		auto resourceToUAVBarrierVelocity = CD3DX12_RESOURCE_BARRIER::Transition(m_velocityTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		auto resourceToUAVBarrierComp = CD3DX12_RESOURCE_BARRIER::Transition(m_compositionTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		auto resourceToUAVBarrierDepth = CD3DX12_RESOURCE_BARRIER::Transition(m_depthTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		auto resourceToUAVBarrierOutput = CD3DX12_RESOURCE_BARRIER::Transition(m_outputTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		m_commandList->Get()->ResourceBarrier(1, &resourceToUAVBarrierOutput);
		m_commandList->Get()->ResourceBarrier(1, &resourceToUAVBarrierComp);
		m_commandList->Get()->ResourceBarrier(1, &resourceToUAVBarrierDepth);
		m_commandList->Get()->ResourceBarrier(1, &resourceToUAVBarrierVelocity);

		FfxFsr2DispatchDescription dispatchParameters = {};
		dispatchParameters.commandList = ffxGetCommandListDX12(m_commandList->Get());
		dispatchParameters.color = ffxGetResourceDX12(m_fsrContext.get(), m_compositionTexture->GetTextureHandle(), L"FSR2_InputColor", FFX_RESOURCE_STATE_UNORDERED_ACCESS);
		dispatchParameters.depth = ffxGetResourceDX12(m_fsrContext.get(), m_depthTexture->GetTextureHandle(), L"FSR2_InputDepth");
		dispatchParameters.motionVectors = ffxGetResourceDX12(m_fsrContext.get(), m_velocityTexture->GetTextureHandle(), L"FSR2_InputMotionVectors");
		dispatchParameters.exposure = ffxGetResourceDX12(m_fsrContext.get(), nullptr, L"FSR2_InputExposure");
		dispatchParameters.reactive = ffxGetResourceDX12(m_fsrContext.get(), nullptr, L"FSR2_EmptyInputReactiveMap");
		dispatchParameters.transparencyAndComposition = ffxGetResourceDX12(m_fsrContext.get(), nullptr, L"FSR2_EmptyTransparencyAndCompositionMap");
		dispatchParameters.output = ffxGetResourceDX12(m_fsrContext.get(), m_outputTexture->GetTextureHandle(), L"FSR2_OutputUpscaledColor", FFX_RESOURCE_STATE_UNORDERED_ACCESS);
		dispatchParameters.jitterOffset.x = m_jitter.x;
		dispatchParameters.jitterOffset.y = m_jitter.y;
		dispatchParameters.motionVectorScale.x = (float)m_manager.GetRenderWidth();
		dispatchParameters.motionVectorScale.y = (float)m_manager.GetRenderHeight();
		dispatchParameters.reset = false;
		dispatchParameters.enableSharpening = true;
		dispatchParameters.sharpness = m_sharpnessValue;
		dispatchParameters.frameTimeDelta = (float)m_deltaTime;
		dispatchParameters.preExposure = 1.0f;
		dispatchParameters.renderSize.width = m_manager.GetRenderWidth();
		dispatchParameters.renderSize.height = m_manager.GetRenderHeight();
		dispatchParameters.cameraFar = 1000.0f;
		dispatchParameters.cameraNear = 1.0f;
		dispatchParameters.cameraFovAngleVertical = 90.0f;
		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "FSR2 System ms");
		FfxErrorCode errorCode = ffxFsr2ContextDispatch(m_fsrContext.get(), &dispatchParameters);
		FFX_ASSERT(errorCode == FFX_OK);
		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);
		auto resourceToPixelShaderResourceBarrierVelocity = CD3DX12_RESOURCE_BARRIER::Transition(m_velocityTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		auto resourceToPixelShaderResourceBarrierComp = CD3DX12_RESOURCE_BARRIER::Transition(m_compositionTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		auto resourceToPixelShaderResourceBarrierDepth = CD3DX12_RESOURCE_BARRIER::Transition(m_depthTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		auto resourceToPixelShaderResourceBarrierOutput = CD3DX12_RESOURCE_BARRIER::Transition(m_outputTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_commandList->Get()->ResourceBarrier(1, &resourceToPixelShaderResourceBarrierComp);
		m_commandList->Get()->ResourceBarrier(1, &resourceToPixelShaderResourceBarrierVelocity);
		m_commandList->Get()->ResourceBarrier(1, &resourceToPixelShaderResourceBarrierDepth);
		m_commandList->Get()->ResourceBarrier(1, &resourceToPixelShaderResourceBarrierOutput);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
		
	}

	void FSR2System::Update()
	{

	}

	void FSR2System::SetUpData()
	{

	}

	void FSR2System::Dispatch()
	{

	}

	void FSR2System::Bind()
	{

	}

	void FSR2System::Unbind()
	{

	}

	void FSR2System::Resize(int width, int height)
	{
		ffxFsr2ContextDestroy(m_fsrContext.get());
		free(m_fsrContextDescription->callbacks.scratchBuffer);

		m_framebuffer->Resize(m_manager.GetViewportWidth(), m_manager.GetViewportHeight());
		m_outputTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_framebuffer->GetFramebufferTexture(0));

		//Grab these again incase the framebuffers have all changed size
		m_compositionTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_lightingSystem->GetFramebuffer()->GetFramebufferTexture(0));
		m_depthTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_forwardSystem->GetFramebuffer()->GetDepthTexture());
		m_velocityTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(3));

		// Setup DX12 interface.
		const size_t scratchBufferSize = ffxFsr2GetScratchMemorySizeDX12();
		void* scratchBuffer = malloc(scratchBufferSize);
		FfxErrorCode errorCode = ffxFsr2GetInterfaceDX12(&m_fsrContextDescription->callbacks, DirectXContext.GetDevice(), scratchBuffer, scratchBufferSize);
		FFX_ASSERT(errorCode == FFX_OK);
		
		// Set up the context description.
		m_fsrContextDescription->device = ffxGetDeviceDX12(DirectXContext.GetDevice());
		m_fsrContextDescription->maxRenderSize.width = m_manager.GetRenderWidth();
		m_fsrContextDescription->maxRenderSize.height = m_manager.GetRenderHeight();
		m_fsrContextDescription->displaySize.width = m_manager.GetViewportWidth();
		m_fsrContextDescription->displaySize.height = m_manager.GetViewportHeight();
		m_fsrContextDescription->flags = FFX_FSR2_ENABLE_HIGH_DYNAMIC_RANGE
			| FFX_FSR2_ENABLE_DEPTH_INVERTED
			| FFX_FSR2_ENABLE_AUTO_EXPOSURE;
		
		// Create the FSR2 context.
		errorCode = ffxFsr2ContextCreate(m_fsrContext.get(), m_fsrContextDescription.get());
		FFX_ASSERT(errorCode == FFX_OK);
	}

	void FSR2System::CalculateJitter()
	{
		m_frameCount++;
		int32_t phaseCount = ffxFsr2GetJitterPhaseCount(m_manager.GetRenderWidth(), m_manager.GetViewportWidth());
		ffxFsr2GetJitterOffset(&m_jitter.x, &m_jitter.y, m_frameCount, phaseCount);

		m_projectionJitter.x = (2.0f * m_jitter.x) / (float)m_manager.GetRenderWidth();
		m_projectionJitter.y = (-2.0f * m_jitter.y) / (float)m_manager.GetRenderHeight();
	}

	void FSR2System::DebugDraw()
	{
		ImGui::Begin("FSR-2 Debug");
		ImGui::Text("Render Resolution: %d x %d", m_manager.GetRenderWidth(), m_manager.GetRenderHeight());
		ImGui::Text("Display Resolution: %d x %d", m_manager.GetViewportWidth(), m_manager.GetViewportHeight());
		ImGui::End();
	}
}