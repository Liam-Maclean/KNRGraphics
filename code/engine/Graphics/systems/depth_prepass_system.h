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
	struct CameraDeferredData;
	class IndirectSignature;
	class Plane;
	class DirectXCommandBuffer;
	class Technique;
	class RenderSystemManager;
	class DepthPrepassSystem : public RenderSystem
	{
	public:
		struct RenderData
		{
			RenderData() { renderables = nullptr; };
			CameraDeferredData cameraSettings;

			RenderBucket* renderBucket;
			std::map<entt::entity, InstancedRenderable>* renderables; //Pointer to the renderables in the scene
			std::map<entt::entity, InstancedCamera>* cameras;
			//std::map<entt::entity, InstancedLight>* lights;
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

		DepthPrepassSystem(RenderSystemManager& manager);
		~DepthPrepassSystem();

		virtual void Init() override;
		virtual void Render() override;
	
		virtual void Update() override;
		virtual void SetUpData() override;
		virtual void Dispatch() override;
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void DebugDraw() override;
		virtual void Resize(int width, int height) override;
	private:
		RenderData m_renderData;
		Ref<QRender::Buffer> m_cameraDataUBO;

		Ref<QRender::IndirectSignature> m_indirectDrawSignature;

		Ref<QRender::Technique> m_technique;
		Ref<QRender::Shader> m_vertShader;
		Ref<QRender::Shader> m_fragShader;
		Ref<QRender::DirectXCommandBuffer> m_commandList;

		Ref<QRender::PBRMaterial> m_defaultMaterial;
		Ref<QRender::PBRMaterial> m_defaultBlackMaterial;

		Ref<QRender::Plane> m_debugGrid;

		DebugStats m_debugStats;

	protected:
	};
}