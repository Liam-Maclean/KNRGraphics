#pragma once
#include "graphics/platform/directx/directx_render_system.h"
#include "graphics/systems/forward_system.h"
#include "graphics/systems/deferred_lighting_system.h"
namespace QRender
{
	struct TAASettings
	{
		float velocityScale;
		float feedbackFactor;
		float maxDepthFalloff;
		int taaEnabled;
	};

	struct FrameSettings;
	class RenderSystemManager;
	class TAASystem : public RenderQuadSystem
	{
		struct RenderData
		{
			TAASettings taaSettings;
			FrameSettings frameSettings;
		};

	public:
		TAASystem(RenderSystemManager& manager);
		~TAASystem();

		virtual void Init() override;
		virtual void Render() override;
		virtual void Update() override;
		virtual void SetUpData() override;
		virtual void Dispatch() override;
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void DebugDraw() override;
		virtual void UpdateFrameSettings(FrameSettings inFrameSettings);
		virtual void UpdateTAASettings(TAASettings inTaaSettings);
		virtual void Resize(int width, int height) override;
		inline Vector2f GetJitterValues() { return m_jitter; }
		inline Vector2f GetProjectionJitter() { return m_projJitter; }
		void CalculateJitter();
	private:
		Ref<QRender::Buffer> m_frameSettingsDataUBO;
		Ref<QRender::Buffer> m_taaSettingsDataUBO;
		Ref<QRender::ForwardSystem> m_forwardSystem;
		Ref<QRender::LightingSystem> m_lightingSystem;

		Ref<QRender::Texture2D> m_compositionTexture;
		Ref<QRender::Texture2D> m_previousCompositionTexture;
		Ref<QRender::Texture2D> m_depthTexture;
		Ref<QRender::Texture2D> m_previousDepthTexture;
		Ref<QRender::Texture2D> m_velocityTexture;

		Ref <QRender::DirectXCommandBuffer> m_commandList;

	protected:
		FrameSettings m_frameSettings;
		RenderData m_renderData;
		Vector2f m_jitter;
		Vector2f m_projJitter;
	};
}