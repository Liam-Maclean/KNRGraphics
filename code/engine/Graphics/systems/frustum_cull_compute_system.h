#pragma once
#include "graphics/platform/directx/directx_render_system.h"

struct RenderBucket;

namespace QRender
{
	class EditorCamera;
	class CameraDeferredData;
	class RenderSystemManager;
	class Technique;
	class FrustumCullComputeSystem : public RenderSystem
	{
	public:
		struct CullData
		{
			glm::mat4 view;
			float znear, zfar; // symmetric projection parameters
			float frustum[4]; // data for left/right/top/bottom frustum planes
			uint32_t drawCount;
		};

		struct RenderData
		{
			RenderData() {
				renderBucket = nullptr; 
				cameraDeferredData = nullptr;
				cullData = nullptr;
			};
			RenderBucket* renderBucket;
			CameraDeferredData* cameraDeferredData;
			CullData* cullData;
		
		};

		FrustumCullComputeSystem(RenderSystemManager& manager);
		~FrustumCullComputeSystem();

		virtual void Init() override;
		virtual void Render() override;	
		virtual void Update() override;
		virtual void SetUpData() override;
		virtual void Dispatch() override;
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void DebugDraw() override;
		virtual void Resize(int width, int height) override;
		void UpdateCamera(Ref<QRender::EditorCamera> cullingCamera);
		void SetRenderBucket(RenderBucket* inRenderBucket);
	private:
		Ref<QRender::Shader> m_computeShader;
		Ref<QRender::Technique> m_technique;
		Ref<QRender::DirectXCommandBuffer> m_computeCommandList;
		Ref<QRender::EditorCamera> m_cullingCamera;

	protected:
		RenderData m_renderData;
	};
}