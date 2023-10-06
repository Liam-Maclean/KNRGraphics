#pragma once

#include "application/base_layer.h"

#include "camera.h"

#include "graphics/platform/directx/directx_system_manager.h"

#include "events/render_events.hpp"

#include "graphics/material.h"
#include "graphics/framebuffer.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/primitives/plane.h"
#include "graphics/platform/directx/directx_gltf_model.h"
#include "graphics/graphics_profiler.h"
#include "graphics/primitives/cube.h"

#include "graphics/systems/forward_system.h"

#include "assets/asset_database.h"
#include "assets/asset_metadata.hpp"

class TextureAsset;
class ModelAsset;

struct InstancedRenderable;
struct InstancedDirectionalLight;
struct InstancedPointLight;
struct InstancedSpotLight;
struct InstancedCamera;
struct InstancedProbe;
class IndirectDrawArgs;
class EditorCameraController;
namespace QCore
{
	struct GLTFStructure;
}

namespace QRender
{
	class Technique;
	class PBRMaterial;
	class EditorCamera;
}

class RenderLayer
	: public BaseLayer
{
public: 
	RenderLayer();

	virtual void OnAttach();
	virtual void OnDetach();
	virtual void OnUpdate(const GameTime& gameTime);
	virtual void OnDevRender(const GameTime& gameTime);
	virtual void OnEvent(const Ref<QCore::Event>& event) override;
	virtual bool OnWindowResized(const QCore::WindowResizeEvent& windowResizedEvent);


	void BuildBatches();
	void FilterBatches(RenderBucket* renderPass);
	void CreateIndirectBatches(RenderBucket* renderPass, std::vector<IndirectBatch>& outBatches, std::vector<RenderBatch>& inObjects);
	void CreateGPUCommands(RenderBucket* renderPass);
	void UploadGPUCommandsToBuffer(RenderBucket* renderPass);

	void InitRenderSystems();
	void UpdateRenderSystems(const GameTime& gameTime);
	void LoadSceneList();

protected:

	void OnCreateEntitiesRequested(const QRender::CreateEntitiesEvent& creatEntitiesEvent);
	void OnDestroyEntitiesRequested(const QRender::DestroyEntitiesEvent& removeEntitiesEvent);
	void OnEntitiesUpdateRequested(const QRender::UpdateEntitiesEvent& updateEntitiesEvent);
	void OnLoadAssetsRequest(const QRender::LoadAssetsRequestEvent& loadAssetsEvent);
	
	void OnCreateRenderComponentsRequested(const QRender::CreateRenderComponentEvent& createRenderComponentsEvent);
	void OnUpdateRenderComponentsRequested(const QRender::UpdateRenderComponentsEvent& updateRenderComponentsEvent);

	void OnCreateDirectionalLightRequested(const QRender::CreateDirectionalLightEvent& createDirectionalLightEvent);
	void OnCreatePointLightRequested(const QRender::CreatePointLightEvent& createPointLightEvent);
	void OnCreateSpotLightRequested(const QRender::CreateSpotLightEvent& createSpotLightEvent);
	void OnCreateCameraRequested(const QRender::CreateCameraEvent& createCameraEvent);

	Ref<IndirectDrawArgs> CreateIndirectDrawArgs(const QRender::StaticMesh& mesh);
	InstancedRenderable* GetInstancedRenderableViaKey(entt::entity key);
	Ref<QRender::Technique> GetGraphicsTechniqueViaKey(AssetMetadata key, AssetMetadata modelKey);
	Ref<QRender::PBRMaterial> GetMaterialViaKey(AssetMetadata key);
	Ref<IndirectDrawArgs> GetIndirectDrawArgsViaKey(AssetMetadata key);	
	void OnCameraUpdated(const QRender::UpdateCameraEvent& cameraData);

private:
	AssetDatabase m_assetDatabase;

	void CommitPendingGPUCopyRequests();
	void CommitPerFrameData();
	void CommitTransformData();

	//Instanced data
	std::vector<Ref<TextureAsset>> m_pendingTextures;
	std::map<entt::entity, QRender::SceneCamera> m_sceneCameras;
	std::map<entt::entity, InstancedRenderable> m_renderables;
	std::map<entt::entity, InstancedDirectionalLight> m_directionalLights;
	std::map<entt::entity, InstancedPointLight> m_pointLights;
	std::map<entt::entity, InstancedSpotLight> m_spotLights;
	std::map<entt::entity, InstancedProbe> m_probes;
	std::map<AssetMetadata, Ref<QRender::Technique>> m_renderTechniques;
	std::map<AssetMetadata, Ref<QRender::PBRMaterial>> m_renderMaterials;
	std::map<AssetMetadata, Ref<IndirectDrawArgs>> m_renderModels;

	uint32_t m_currentIndexCount, m_currentVertexCount;

	//Draw batch buckets
	RenderBucket m_forwardRenderBatchBucket;
	RenderBucket m_shadowRenderBatchBucket;
	RenderBucket m_transparentRenderBatchBucket;

	//Scene data
	Ref<QRender::EditorCamera> m_editorCamera;
	Ref<QRender::EditorCamera> m_directionalLightOrthoCamera;
	Ref<EditorCameraController> m_cameraController;

	//Render systems and debug
	Ref<QRender::RenderSystemManager> m_renderManager;

	//Settings
	QRender::CameraDeferredData cameraData = {};
	QRender::FrameSettings frameSettings = {};
	float animationTimer = 0;
	float deltaTime = 0.085f;
};