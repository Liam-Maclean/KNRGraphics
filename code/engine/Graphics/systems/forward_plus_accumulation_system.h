#pragma once
#include "graphics/renderable_types.h"
#include "graphics/platform/directx/directx_render_system.h"
#include "graphics/systems/forward_system.h"
#include "graphics/mesh.h"

namespace QCore
{
	class Node;
	struct GLTFStructure;
}

namespace QRender
{
	class EditorCamera;
	struct CameraDeferredData;

	class IndirectSignature;
	class Plane;
	class DirectXCommandBuffer;
	class Technique;
	class RenderSystemManager;
	class ForwardPlusAccumulationSystem : public RenderSystem
	{
	public:
		struct RenderData
		{
			RenderData() { renderables = nullptr; };
			CameraDeferredData cameraSettings;

			RenderBucket* renderBucket;
			std::map<entt::entity, InstancedRenderable>* renderables; //Pointer to the renderables in the scene
			std::map<entt::entity, InstancedCamera>* cameras;
			std::map<entt::entity, InstancedDirectionalLight>* directionalLights;
			std::map<entt::entity, InstancedPointLight>* pointLights;
			std::map<entt::entity, InstancedSpotLight>* spotLights;
			std::map<entt::entity, InstancedProbe>* probes;

			std::vector<Ref<QRender::StaticMesh>> staticMeshes;
			std::vector<Ref<QCore::GLTFStructure>> instances;
		};

		struct DebugStats
		{
			int triangleCount;
			int objectCount;
			int drawCalls;
		};

		ForwardPlusAccumulationSystem(RenderSystemManager& manager);
		~ForwardPlusAccumulationSystem();

		virtual void Init() override;
		virtual void Render() override;
	
		virtual void Update() override;
		virtual void SetUpData() override;
		virtual void Dispatch() override;
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void DebugDraw() override;
		virtual void Resize(int width, int height) override;
		void UpdateCameraData(CameraDeferredData inCameraData);
		void SetRenderBucket(RenderBucket* inRenderBucket);
	private:
		Ref<QRender::Buffer> m_shadowCameraDataUBO;
		RenderData m_renderData;

		Ref<QRender::Buffer> m_cameraDataUBO;
		Ref<QRender::Buffer> m_frameSettingsDataUBO;
		Ref<QRender::Buffer> m_directionalLightBuffer;
		Ref<QRender::Buffer> m_pointLightBuffer;
		Ref<QRender::Buffer> m_spotLightBuffer;

		Ref<QRender::IndirectSignature> m_indirectDrawSignature;

		Ref<QRender::Plane> m_debugGrid;
		Ref<QRender::DirectXCommandBuffer> m_commandList;

		DebugStats m_debugStats;

	protected:
	};
}