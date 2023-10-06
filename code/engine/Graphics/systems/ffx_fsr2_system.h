#pragma once
#include "graphics/platform/directx/directx_render_system.h"

class FfxFsr2Context;
class FfxFsr2ContextDescription;

namespace QRender
{
	enum class FSRUpscaleMode
	{
		NONE,
		ULTRA_QUALITY,
		QUALITY,
		BALANCE,
		PERFORMANCE,
		ULTRA_PERFORMANCE
	};

	class ForwardSystem;
	class LightingSystem;
	class DirectXTexture2D;
	class RenderSystemManager;
	class FSR2System : public RenderQuadSystem
	{
	public:
		FSR2System(RenderSystemManager& manager);
		~FSR2System();

		virtual void Init() override;
		virtual void Render() override;	
		virtual void Update() override;
		virtual void SetUpData() override;
		virtual void Dispatch() override;
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Resize(int width, int height) override;
		virtual void DebugDraw() override;
		inline Vector2f GetJitterValues() { return m_jitter; }
		inline Vector2f GetProjectionJitter() { return m_projectionJitter; }
		void CalculateJitter();
	private:
		
		float GetFSRUpscaleRatio(FSRUpscaleMode upscaleMode);

		
		Ref<FfxFsr2Context> m_fsrContext;
		Ref<FfxFsr2ContextDescription> m_fsrContextDescription;

		Ref<QRender::ForwardSystem> m_forwardSystem;
		Ref<QRender::LightingSystem> m_lightingSystem;

		Ref<QRender::DirectXTexture2D> m_compositionTexture;
		Ref<QRender::DirectXTexture2D> m_depthTexture;
		Ref<QRender::DirectXTexture2D> m_velocityTexture;
		Ref<QRender::DirectXTexture2D> m_outputTexture;

		Ref<QRender::DirectXCommandBuffer> m_commandList;

	protected:
		int m_sharpnessValue = 1.0f; // 0 to 1
		Vector2f m_jitter;
		Vector2f m_projectionJitter;
		float m_deltaTime;
		int m_frameCount;
	};
}