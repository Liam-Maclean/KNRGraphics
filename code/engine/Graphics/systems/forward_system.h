#pragma once
#include "graphics/renderable_types.h"
#include "graphics/platform/directx/directx_render_system.h"
#include "graphics/mesh.h"
namespace QCore
{
	class Node;
	struct GLTFStructure;
}
struct RenderBucket;

namespace QRender
{
	class IndirectSignature;
	class Plane;
	class DirectXCommandBuffer;
	struct FrameSettings
	{
		Vector2f resolution;
		float deltaTime;
		float elapsedTime;
		int totalFrames;
	};

	struct CameraDeferredData
	{
		Mat4x4  viewMat;
		Mat4x4  projMat;
		Mat4x4  modelMat;
		Mat4x4	prevViewMat;
		Mat4x4	prevProjMat;
		Mat4x4	prevModelMat;
		Mat4x4  inverseViewMat;
		Mat4x4  inverseProjMat;
		Mat4x4  invertedTransposeModelMat;
		Mat4x4  directionalLightMat;
		Vector4f cameraPos;
		Vector2f previousJitter;
		Vector2f currentJitter;
	};

	struct JitterSettings
	{
		Vector2f jitter;
		Vector2f prevJitter;
		float haltonScale;

		JitterSettings()
		{
			haltonScale = 100.0f;
		}
	};
	
	class Technique;
	class RenderSystemManager;
	class ForwardSystem : public RenderSystem
	{
	public:
		struct RenderData
		{
			RenderData() { renderables = nullptr; };
			FrameSettings frameSettings;
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

		ForwardSystem(RenderSystemManager& manager);
		virtual ~ForwardSystem();

		virtual void Init() override;
		virtual void Render() override;
		virtual void Update() override;
		virtual void SetUpData() override;
		virtual void Dispatch() override;
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Resize(int width, int height) override;
		virtual void DebugDraw() override;
		void UpdateCameraData(CameraDeferredData inCameraData);
		void UpdateShaderData(RenderData forwardShaderData);
		void UpdateFrameSettings(FrameSettings inFrameSettings);
		void SetRenderBucket(RenderBucket* inRenderBucket);
		// Inherited via IRenderListSystem
		virtual void SetRenderables(std::map<entt::entity, InstancedRenderable>& renderables);
		inline void SetWireframeEnabled(bool enabled) { m_wireframeMode = enabled; }
		inline void SetDebugGridEnabled(bool enabled) { m_debugGridEnabled = enabled; }
	private:
		void DrawNode(QCore::Node* node);
		void Draw(QCore::GLTFStructure& gltfModel);
		void DrawStatic(const QRender::StaticMesh& staticMesh);

		Ref<QRender::Buffer> m_cameraDataUBO;
		Ref<QRender::Buffer> m_frameSettingsDataUBO;

		Ref<QRender::Shader> m_vertShader;
		Ref<QRender::Shader> m_fragShader;

		Ref<QRender::Technique> m_technique;
		Ref<QRender::Technique> m_wireframeTechnique;
		
		Ref<QRender::IndirectSignature> m_indirectDrawSignature;

		Ref<QRender::PBRMaterial> m_defaultMaterial;
		Ref<QRender::PBRMaterial> m_defaultBlackMaterial;

		Ref<QRender::Plane> m_debugGrid;

		Ref<QRender::DirectXCommandBuffer> m_commandList;
		glm::mat4 m_gridTransform;

		DebugStats m_debugStats;

	protected:
		RenderData m_renderData;
		bool m_wireframeMode = false;
		bool m_debugGridEnabled = true;
		bool m_debugWindowEnabled = true;
	};
}