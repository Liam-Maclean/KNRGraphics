#pragma once
#include "graphics/renderable_types.h"
#include "graphics/platform/directx/directx_render_system.h"
#include "graphics/mesh.h"

namespace QCore
{
	class Node;
	struct GLTFStructure;
}

namespace QRender
{
	struct CameraOrthoData
	{
		Mat4x4 viewMat;
		Mat4x4 projMat;
		Mat4x4 modelMat;
		Mat4x4 invertedTransposeModelMat;
	};
	class Technique;
	class RenderSystemManager;
	class ShadowSystem : public RenderSystem
	{
	public:
		struct RenderData
		{
			RenderData() { renderables = nullptr; };
			CameraOrthoData cameraData;
			std::map<entt::entity, InstancedRenderable>* renderables; //Pointer to the renderables in the scene
		};

		ShadowSystem(RenderSystemManager& manager);
		~ShadowSystem();

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
		virtual void UpdateCameraData(CameraOrthoData inCameraData);
		virtual void SetRenderables(std::map<entt::entity, InstancedRenderable>& renderables);
		void DrawStatic(const QRender::StaticMesh& staticMesh);
		void DrawNode(QCore::Node* node);
		void Draw(QCore::GLTFStructure& gltfModel);
	private:
		Ref<QRender::Buffer> m_shadowCameraDataUBO;
		RenderData m_renderData;

		Ref<QRender::Technique> m_technique;
		Ref<QRender::Shader> m_vertShader;
		Ref<QRender::DirectXCommandBuffer> m_commandList;

	protected:
	};
}