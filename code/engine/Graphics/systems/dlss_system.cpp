#include <qlpch.h>
#include "dlss_system.h"
#include "graphics/platform/directx/directx_texture.h"
#include "graphics/systems/deferred_lighting_system.h"
#include "graphics/systems/forward_system.h"
#include "graphics/systems/deferred_lighting_system.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/graphics_profiler.h"
#include "graphics/render_commands.h"
#include "graphics/platform/directx/d3dx12.h"

//DLSS libraries
#include "dlss/include/nvsdk_ngx.h"
#include "dlss/include/nvsdk_ngx_helpers.h"
#include "dlss/include/nvsdk_ngx_params.h"

namespace QRender
{
	DLSSSystem::DLSSSystem(RenderSystemManager& manager)
		: RenderQuadSystem(manager)
	{

	}

	DLSSSystem::~DLSSSystem()
	{

	}

	void DLSSSystem::Init()
	{
		Q_RENDER_INFO("INITIALISING DLSS SYSTEM");

		m_commandList = MakeRef<QRender::DirectXCommandBuffer>(CommandBufferType::graphics);
		m_commandList->Close();

		m_frameCount = 0;
		QRender::FramebufferSpecification fbSpec;
		fbSpec.Attachments =
		{
			QRender::FramebufferTextureFormat::RGBA8,
		};
		fbSpec.Width = 1920;
		fbSpec.Height = 1080;
		m_framebuffer = QRender::Framebuffer::Create(fbSpec);

		//Ultra Quality
		m_manager.SetUpscaleRatio(GetDLSSUpscaleRatio());

		m_forwardSystem = m_manager.GetRenderSystem<QRender::ForwardSystem>();
		m_lightingSystem = m_manager.GetRenderSystem<QRender::LightingSystem>();
		m_compositionTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_lightingSystem->GetFramebuffer()->GetFramebufferTexture(0));
		m_depthTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_forwardSystem->GetFramebuffer()->GetDepthTexture());
		m_velocityTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(3));
		m_outputTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_framebuffer->GetFramebufferTexture(0));

		int dlssSupported = 0;
		int needsUpdatedDriver = 0;
		unsigned int minDriverVersionMajor = 0;
		unsigned int minDriverVersionMinor = 0;

		//DO DLSS INITIALISATION HERE
		NVSDK_NGX_Result ngxInitResult;
		ngxInitResult = NVSDK_NGX_D3D12_Init(0, L"Assets/",DirectXContext.GetDevice());
		if (NVSDK_NGX_FAILED(ngxInitResult))
		{
			//DLSS Initialisation failed
		}

		NVSDK_NGX_Result ngxGetCapabilityParamsResult;
		ngxGetCapabilityParamsResult = NVSDK_NGX_D3D12_AllocateParameters(&m_parameters);
		if (NVSDK_NGX_FAILED(ngxGetCapabilityParamsResult))
		{
			//Failed to get capability parameters, something went wrong in initialization
		}

		NVSDK_NGX_Result resultUpdatedDriver = m_parameters->Get(NVSDK_NGX_Parameter_SuperSampling_NeedsUpdatedDriver, &needsUpdatedDriver);
		if (NVSDK_NGX_FAILED(resultUpdatedDriver))
		{

		}


		NVSDK_NGX_Result ResultMinDriverVersionMajor = m_parameters->Get(NVSDK_NGX_Parameter_SuperSampling_MinDriverVersionMajor, &minDriverVersionMajor);
		NVSDK_NGX_Result ResultMinDriverVersionMinor = m_parameters->Get(NVSDK_NGX_Parameter_SuperSampling_MinDriverVersionMinor, &minDriverVersionMinor);



		NVSDK_NGX_Result dlssSupportedResult = m_parameters->Get(NVSDK_NGX_Parameter_SuperSampling_Available, &dlssSupported);
		if (NVSDK_NGX_FAILED(dlssSupportedResult) || !dlssSupported)
		{
			//dlss is not supported by this device, we shouldn't do any more and crash here
		}

		//Need a selector for DLSS quality mode
		NVSDK_NGX_DLSS_Create_Params dlssParams = {};
		dlssParams.Feature.InWidth = m_manager.GetRenderWidth();
		dlssParams.Feature.InHeight = m_manager.GetRenderHeight();
		dlssParams.Feature.InTargetWidth = m_manager.GetViewportWidth();
		dlssParams.Feature.InTargetHeight = m_manager.GetViewportHeight();

		NVSDK_NGX_Result createDLSSFeature = NGX_D3D12_CREATE_DLSS_EXT(DirectXContext.GetCommandList(), 1, 1, &m_featureHandle, m_parameters, &dlssParams);
		if (NVSDK_NGX_FAILED(createDLSSFeature))
		{
		}

		uint64_t texId = m_framebuffer->GetColorAttachmentRendererID();
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = DirectXContext.GetImGuiHeap()->handleGPU(texId);
		QRender::RenderCommand::SetFinalRenderTextureId(reinterpret_cast<void*>(textureHandle.ptr));
	}

	void DLSSSystem::Render()
	{
		//Submit work immediately (queue is deferred from type)
		m_commandList->Reset();
		int frametimeIdx = m_manager.GetGraphicsProfiler()->StartProfiler(m_commandList, "DLSS System ms");

		m_deltaTime = 0.085f;
		
		auto resourceToUAVBarrierVelocity = CD3DX12_RESOURCE_BARRIER::Transition(m_velocityTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		auto resourceToUAVBarrierComp = CD3DX12_RESOURCE_BARRIER::Transition(m_compositionTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		auto resourceToUAVBarrierDepth = CD3DX12_RESOURCE_BARRIER::Transition(m_depthTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		auto resourceToUAVBarrierOutput = CD3DX12_RESOURCE_BARRIER::Transition(m_outputTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		m_commandList->Get()->ResourceBarrier(1, &resourceToUAVBarrierOutput);
		m_commandList->Get()->ResourceBarrier(1, &resourceToUAVBarrierComp);
		m_commandList->Get()->ResourceBarrier(1, &resourceToUAVBarrierDepth);
		m_commandList->Get()->ResourceBarrier(1, &resourceToUAVBarrierVelocity);

		NVSDK_NGX_D3D12_DLSS_Eval_Params params = {};
		params.Feature.pInColor = m_compositionTexture->GetTextureHandle();
		params.Feature.pInOutput = m_outputTexture->GetTextureHandle();
		params.pInDepth = m_depthTexture->GetTextureHandle();
		params.pInMotionVectors = m_velocityTexture->GetTextureHandle();
		params.InRenderSubrectDimensions.Width = m_manager.GetRenderWidth();
		params.InRenderSubrectDimensions.Height = m_manager.GetRenderHeight();
		params.InJitterOffsetX = m_jitter.x;
		params.InJitterOffsetY = m_jitter.y;

		NGX_D3D12_EVALUATE_DLSS_EXT(m_commandList->Get(), m_featureHandle, m_parameters, &params);

		auto resourceToPixelShaderResourceBarrierVelocity = CD3DX12_RESOURCE_BARRIER::Transition(m_velocityTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		auto resourceToPixelShaderResourceBarrierComp = CD3DX12_RESOURCE_BARRIER::Transition(m_compositionTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		auto resourceToPixelShaderResourceBarrierDepth = CD3DX12_RESOURCE_BARRIER::Transition(m_depthTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		auto resourceToPixelShaderResourceBarrierOutput = CD3DX12_RESOURCE_BARRIER::Transition(m_outputTexture->GetTextureHandle(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_commandList->Get()->ResourceBarrier(1, &resourceToPixelShaderResourceBarrierComp);
		m_commandList->Get()->ResourceBarrier(1, &resourceToPixelShaderResourceBarrierVelocity);
		m_commandList->Get()->ResourceBarrier(1, &resourceToPixelShaderResourceBarrierDepth);
		m_commandList->Get()->ResourceBarrier(1, &resourceToPixelShaderResourceBarrierOutput);

		m_manager.GetGraphicsProfiler()->EndProfiler(m_commandList, frametimeIdx);

		//Submit work immediately (queue is deferred from type)
		m_commandList->SubmitWorkImmediate();
	}

	void DLSSSystem::Update()
	{

	}

	void DLSSSystem::SetUpData()
	{

	}

	void DLSSSystem::Dispatch()
	{

	}

	void DLSSSystem::Bind()
	{

	}

	void DLSSSystem::Unbind()
	{

	}

	void DLSSSystem::Resize(int width, int height)
	{
		int dlssSupported = 0;
		NVSDK_NGX_D3D12_ReleaseFeature(m_featureHandle);


		NVSDK_NGX_Result ngxGetCapabilityParamsResult;
		ngxGetCapabilityParamsResult = NVSDK_NGX_D3D12_GetCapabilityParameters(&m_parameters);
		if (ngxGetCapabilityParamsResult != NVSDK_NGX_Result_Success)
		{
			//Failed to get capability parameters, something went wrong in initialization
		}

		NVSDK_NGX_Result dlssSupportedResult = m_parameters->Get(NVSDK_NGX_Parameter_SuperSampling_Available, &dlssSupported);
		if (dlssSupportedResult != NVSDK_NGX_Result_Success || !dlssSupported)
		{
			//dlss is not supported by this device, we shouldn't do any more and crash here
		}

		//Need a selector for DLSS quality mode
		NVSDK_NGX_DLSS_Create_Params dlssParams = {};
		dlssParams.Feature.InWidth = m_manager.GetRenderWidth();
		dlssParams.Feature.InHeight = m_manager.GetRenderHeight();
		dlssParams.Feature.InTargetWidth = m_manager.GetViewportWidth();
		dlssParams.Feature.InTargetHeight = m_manager.GetViewportHeight();
		dlssParams.Feature.InPerfQualityValue = NVSDK_NGX_PerfQuality_Value_MaxQuality;

		NVSDK_NGX_Result createDLSSFeature = NGX_D3D12_CREATE_DLSS_EXT(DirectXContext.GetCommandList(), 1, 1, &m_featureHandle, m_parameters, &dlssParams);
		if (createDLSSFeature != NVSDK_NGX_Result_Success)
		{
		}


		m_framebuffer->Resize(m_manager.GetViewportWidth(), m_manager.GetViewportHeight());
		m_outputTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_framebuffer->GetFramebufferTexture(0));

		//Grab these again incase the framebuffers have all changed size
		m_compositionTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_lightingSystem->GetFramebuffer()->GetFramebufferTexture(0));
		m_depthTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_forwardSystem->GetFramebuffer()->GetDepthTexture());
		m_velocityTexture = std::static_pointer_cast<QRender::DirectXTexture2D>(m_forwardSystem->GetFramebuffer()->GetFramebufferTexture(3));
	}

	void DLSSSystem::CalculateJitter()
	{
		m_frameCount++;
		//index is the total frames modulo sample count
		int index = m_frameCount % 128;

		//Calculate Halton Jitter for TAA
		float haltonX = QMaths::Halton(index + 1, 2);
		float haltonY = QMaths::Halton(index + 1, 3);
		m_jitter = Vector2f(haltonX, haltonY);
		m_projectionJitter.x = (2.0f * m_jitter.x) / (float)m_manager.GetRenderWidth();
		m_projectionJitter.y = (-2.0f * m_jitter.y) / (float)m_manager.GetRenderHeight();
	}

	void DLSSSystem::DebugDraw()
	{
		ImGui::Begin("DLSS DEBUG");
		ImGui::Text("Render Resolution: %d x %d", m_manager.GetRenderWidth(), m_manager.GetRenderHeight());
		ImGui::Text("Display Resolution: %d x %d", m_manager.GetViewportWidth(), m_manager.GetViewportHeight());
		ImGui::End();
	}

	float DLSSSystem::GetDLSSUpscaleRatio()
	{
		return 1.3f;
	}

}