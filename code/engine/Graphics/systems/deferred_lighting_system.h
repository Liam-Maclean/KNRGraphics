#pragma once
#include "graphics/platform/directx/directx_render_system.h"
#include "graphics/systems/forward_system.h"
namespace QRender
{

	struct ShaderDebugValues
	{
		int HDREnabled;
		int GammaCorrectionEnabled;
		int viewMode;
		int iblEnabled;
		int shadowsEnabled;
		int skyboxEnabled;
	};

	struct DirectionalLightData
	{
		Vector4f lightPos;
		Vector4f lightDir;
		Vector4f color;
	};

	struct CameraDeferredData;
	class ShadowSystem;
	class ForwardSystem;
	class RenderSystemManager;
	class Technique;
	class LightingSystem : public RenderQuadSystem
	{
	public:
		struct RenderData
		{
			CameraDeferredData cameraData;
			DirectionalLightData directionalLightData;
			ShaderDebugValues shaderDebugValuesData;
		};

		LightingSystem(RenderSystemManager& manager);
		virtual ~LightingSystem();

		virtual void Init() override;
		virtual void Render() override;
		virtual void Update() override;
		virtual void SetUpData() override;
		virtual void Dispatch() override;
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void DebugDraw() override;
		virtual void Resize(int width, int height) override;
		virtual void UpdateShaderData(RenderData renderData);
		virtual void UpdateCameraData(CameraDeferredData cameraDeferredData);
		virtual void UpdateLightData(DirectionalLightData directionalLightData);
		virtual void UpdateDebugSettings(ShaderDebugValues shaderDebugValues);
	private:


		Ref<QRender::Texture2D> m_gBufferAlbedo;
		Ref<QRender::Texture2D> m_gBufferNormal;
		Ref<QRender::Texture2D> m_gBufferEmissive;
		Ref<QRender::Texture2D> m_gBufferDepth;
		Ref<QRender::Texture2D> m_skybox;
		Ref<QRender::Texture2D> m_brdfLUT;
		Ref<QRender::Texture2D> m_shadowMap;

		Ref<QRender::ShadowSystem> m_shadowSystem;
		Ref<QRender::ForwardSystem> m_forwardSystem;

		Ref<QRender::Buffer> m_cameraDataUBO;
		Ref<QRender::Buffer> m_directionalLightDataUBO;
		Ref<QRender::Buffer> m_shaderDebugValuesDataUBO;

		Ref<QRender::Shader> m_vertShader;
		Ref<QRender::Shader> m_fragShader;

		Ref<QRender::Technique> m_technique;

		Ref<QRender::DirectXCommandBuffer> m_commandList;

		CameraDeferredData m_cameraData;
		DirectionalLightData m_directionalLightData;
		ShaderDebugValues m_shaderDebugValuesData;

		RenderData m_renderData;

	protected:
	};
}