#include "render_layer.h"
#include <future>
#include <imgui.h>
#include <pix.h>

#include "graphics/buffer.h"
#include "graphics/camera.h"
#include "graphics/camera_controller.hpp"
#include "graphics/render_commands.h"
#include "graphics/renderable_types.h"
#include "graphics/platform/directx/directx_frame_heap.h"
#include "graphics/platform/directx/directx_render_system.h"
#include "graphics/platform/directx/directx_system_manager.h"
#include "graphics/platform/directx/directx_technique.h"
#include "graphics/platform/directx/directx_pipeline_layout.h"


#include "systems/fxaa_system.h"
#include "systems/forward_system.h"
#include "systems/shadow_system.h"
#include "systems/deferred_lighting_system.h"
#include "systems/taa_system.h"
#include "systems/ffx_fsr2_system.h"
#include "systems/dlss_system.h"
#include "systems/forward_plus_accumulation_system.h"
#include "systems/depth_prepass_system.h"

#include "mesh.h"
#include "assets/asset_meta_types.h"
#include "assets/asset_loader.h"
#include "assets/texture_asset.h"
#include "assets/model_asset.h"
#include "assets/shader_asset.h"
#include "assets/technique_asset.h"


#include "application/application.h"

const QCore::EventType QRender::LoadAssetRequestEventType	= QCore::EventType("Load Assets",			typeid(QRender::LoadAssetsRequestEvent));
const QCore::EventType QRender::CreateEntitiesEventType		= QCore::EventType("Create Entity Batch",	typeid(QRender::CreateEntitiesEvent));
const QCore::EventType QRender::CreateRenderComponentEventType = QCore::EventType("Create Render Component", typeid(QRender::CreateRenderComponentEvent));
const QCore::EventType QRender::CreateShadersEventType		= QCore::EventType("Create Shader",			typeid(QRender::CreateShadersEvent));
const QCore::EventType QRender::DestroyEntitiesEventType	= QCore::EventType("Destroy Entity Batch",	typeid(QRender::DestroyEntitiesEvent));
const QCore::EventType QRender::UpdateEntitiesEventType		= QCore::EventType("Update Entity Batch",	typeid(QRender::UpdateEntitiesEvent));
const QCore::EventType QRender::UpdateRenderComponentsType	= QCore::EventType("Update Render Component", typeid(QRender::UpdateRenderComponentsEvent));
const QCore::EventType QRender::UpdateCameraEventType		= QCore::EventType("Update Camera Data",	typeid(QRender::UpdateCameraEvent));

const QCore::EventType QRender::CreateCameraEventType		= QCore::EventType("Create Camera", typeid(QRender::CreateCameraEventType));

const QCore::EventType QRender::CreateDirectionalLightEventType = QCore::EventType("Create Directional Light", typeid(QRender::CreateDirectionalLightEvent));
const QCore::EventType QRender::CreatePointLightEventType	= QCore::EventType("Create Point Light", typeid(QRender::CreatePointLightEvent));
const QCore::EventType QRender::CreateSpotLightEventType	= QCore::EventType("Create Directional Light", typeid(QRender::CreateSpotLightEvent));

RenderLayer::RenderLayer()
	: BaseLayer("Render Layer")
{
	m_eventMask = {
		&QCore::WindowResizeEventType,
		&QCore::WindowMovedEventType,
		&QRender::LoadAssetRequestEventType,
		&QRender::CreateEntitiesEventType,
		&QRender::CreateRenderComponentEventType,
		&QRender::CreateShadersEventType,
		&QRender::DestroyEntitiesEventType,
		&QRender::UpdateEntitiesEventType,
		&QRender::UpdateRenderComponentsType,
		&QRender::UpdateCameraEventType,
		&QRender::CreateCameraEventType,
		&QRender::CreateDirectionalLightEventType,
		&QRender::CreatePointLightEventType,
		&QRender::CreateSpotLightEventType,
	};
}

void RenderLayer::OnAttach()
{
	BaseLayer::OnAttach();

	QRender::RenderCommand::Init();

	m_forwardRenderBatchBucket.type = MeshRenderPassType::Forward;
	m_shadowRenderBatchBucket.type = MeshRenderPassType::Shadows;
	m_transparentRenderBatchBucket.type = MeshRenderPassType::Transparency;

	m_forwardRenderBatchBucket.Init();
	m_shadowRenderBatchBucket.Init();
	m_transparentRenderBatchBucket.Init();

	LoadSceneList();

	//Pre allocate 150000 transforms into the forwards structure buffer
	QRender::BufferDescriptor structuredBufferDesc = {};
	structuredBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
	structuredBufferDesc.bufferType = QRender::BufferUsageType::STORAGE;
	structuredBufferDesc.size = 150000 * sizeof(glm::mat4);
	structuredBufferDesc.structuredBuffer.numElements = 150000;
	structuredBufferDesc.structuredBuffer.structuredByteStride = sizeof(glm::mat4);
	structuredBufferDesc.debugName = L"Transform Structured Buffer";

	m_forwardRenderBatchBucket.transformStructuredBuffer = QRender::Buffer::Create(structuredBufferDesc);

	//Reserve 40MB each for the scene vertex and index buffers
	QRender::BufferDescriptor sceneVertexBufferDesc = {};
	sceneVertexBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
	sceneVertexBufferDesc.bufferType = QRender::BufferUsageType::VERTEX;
	sceneVertexBufferDesc.vertexBuffer.stride = sizeof(QRender::PrimitiveVertex);
	sceneVertexBufferDesc.size = 1024 * 1024 * 40; //40mb
	sceneVertexBufferDesc.debugName = L"Scene vertex buffer";
	m_forwardRenderBatchBucket.sceneCombinedVertexBuffer = QRender::Buffer::Create(sceneVertexBufferDesc);

	QRender::BufferDescriptor sceneIndexBufferDesc = {};
	sceneIndexBufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
	sceneIndexBufferDesc.bufferType = QRender::BufferUsageType::INDEX;
	sceneIndexBufferDesc.size = 1024 * 1024 * 40; //40mb
	sceneIndexBufferDesc.debugName = L"Scene index buffer";
	m_forwardRenderBatchBucket.sceneCombinedIndexBuffer = QRender::Buffer::Create(sceneIndexBufferDesc);
	
	m_currentIndexCount = 0;
	m_currentVertexCount = 0;

	unsigned int width = Application::GetWindow().GetWidth();
	unsigned int height = Application::GetWindow().GetHeight();

	m_editorCamera = MakeRef<QRender::EditorCamera>(QRender::EProjectionType::Perspective);
	m_editorCamera->SetViewportSize({ width, height });

	m_cameraController = MakeRef<EditorCameraController>();
	m_cameraController->SetCamera(m_editorCamera);

	m_directionalLightOrthoCamera = MakeRef<QRender::EditorCamera>(QRender::EProjectionType::Orthographic);
	m_directionalLightOrthoCamera->SetViewportSize({ width, height });
	m_cameraController->Resize(width, height);

	ScreenQuadObject;

	m_renderManager = MakeRef<QRender::RenderSystemManager>();
	m_renderManager->Resize(width, height);

	InitRenderSystems();

	QRender::RenderCommand::SetViewport(0, 0, width, height);
	// LaunchThread();
}

void RenderLayer::OnDetach() 
{

}

void RenderLayer::OnUpdate(const GameTime& gameTime)
{
	//Commit any thread stopping GPU copy requests before rendering
	CommitPendingGPUCopyRequests();

	//We should be doing this per frame (transforms, materials etc)
	CommitPerFrameData();

	BuildBatches();

	QRender::RenderCommand::BeginRender();

	//This needs to be moved to the game layer? Cameras could be sent with game events to the render layer for this data
	//This can be moved to the ECS/Scene system except for the editor camera (The one used for the editor and not in scene)
	m_cameraController->Update(gameTime.TimeStep);
	m_editorCamera->OnUpdate(gameTime);
	m_directionalLightOrthoCamera->OnUpdate(gameTime);

	UpdateRenderSystems(gameTime);
	
	//Rendering 
	auto forwardAccumulationSystem = m_renderManager->GetRenderSystem<QRender::ForwardPlusAccumulationSystem>();
	if (forwardAccumulationSystem)
	{
		forwardAccumulationSystem->Render();
	}

	QRender::RenderCommand::EndRender(); //geometry has ended

	//This is how we should expect prod to call present
	//QRender::RenderCommand::BlitToSwapchain(m_FXAAFinalOutput);
	//QRender::RenderCommand::Present();
}

void RenderLayer::OnDevRender(const GameTime& gameTime)
{
	
	m_renderManager->DebugDraw();
	m_renderManager->DrawFrametimes();
}

void RenderLayer::InitRenderSystems()
{
	auto forwardAccumulationSystem = m_renderManager->CreateRenderSystem<QRender::ForwardPlusAccumulationSystem>();

	forwardAccumulationSystem->Init();
	forwardAccumulationSystem->SetColorOutputAsFinalRenderTarget();
	forwardAccumulationSystem->SetRenderBucket(&m_forwardRenderBatchBucket);
}

void RenderLayer::UpdateRenderSystems(const GameTime& gameTime)
{
	frameSettings.resolution = { 1920.0f, 1080.0f };
	frameSettings.totalFrames++;
	frameSettings.elapsedTime = gameTime.CurrentTick;
	frameSettings.deltaTime = gameTime.TimeStep.getTime();

	//Set previous frame data for TAA
	cameraData.prevModelMat = cameraData.modelMat;
	cameraData.prevProjMat = cameraData.projMat;
	cameraData.prevViewMat = cameraData.viewMat;
	cameraData.previousJitter = cameraData.currentJitter;
	
	cameraData.viewMat = glm::transpose(m_editorCamera->GetView());
	cameraData.projMat = glm::transpose(m_editorCamera->GetProjection());
	cameraData.modelMat = glm::mat4(1.0f);
	cameraData.modelMat = glm::scale(cameraData.modelMat, glm::vec3(10.0f));
	cameraData.modelMat = glm::rotate(cameraData.modelMat, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	cameraData.inverseProjMat = glm::inverse(cameraData.projMat);
	cameraData.inverseViewMat = glm::inverse(cameraData.viewMat);
	cameraData.invertedTransposeModelMat = glm::inverse(cameraData.modelMat);
	cameraData.directionalLightMat = m_directionalLightOrthoCamera->GetProjection() * m_directionalLightOrthoCamera->GetView();
	cameraData.directionalLightMat = glm::transpose(cameraData.directionalLightMat);
	cameraData.cameraPos = Vector4f(m_editorCamera->GetPosition(), 1.0f);
	cameraData.previousJitter = m_editorCamera->GetPreviousJitter();
	cameraData.currentJitter = m_editorCamera->GetCurrentJitter();

	/* todo: handle dirty flag via events */
	//QRender::CameraOrthoData shadowOrthoCameraData = {};
	//shadowOrthoCameraData.modelMat = glm::mat4(1.0f);
	//shadowOrthoCameraData.modelMat = glm::scale(shadowOrthoCameraData.modelMat, glm::vec3(10.0f));
	//shadowOrthoCameraData.modelMat = glm::rotate(shadowOrthoCameraData.modelMat, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//shadowOrthoCameraData.viewMat = glm::transpose(m_directionalLightOrthoCamera->GetView());
	//shadowOrthoCameraData.projMat = glm::transpose(m_directionalLightOrthoCamera->GetProjection());
	//shadowOrthoCameraData.invertedTransposeModelMat = glm::inverse(shadowOrthoCameraData.modelMat);
	
	auto forwardAccumulationSystem = m_renderManager->GetRenderSystem<QRender::ForwardPlusAccumulationSystem>();

	if (forwardAccumulationSystem)
	{
		forwardAccumulationSystem->UpdateCameraData(cameraData);
	}
}

void RenderLayer::LoadSceneList()
{
	//Scan the full directory for assets
	m_assetDatabase.ScanDirectoryForAssets("assets/shaders");
}

void RenderLayer::OnEvent(const Ref<QCore::Event>& event)
{
	if (event->GetType() == QCore::WindowResizeEventType)
	{
		OnWindowResized(event->Value<QCore::WindowResizeEvent>());
	}
	else if (event->GetType() == QRender::LoadAssetRequestEventType)
	{
		OnLoadAssetsRequest(event->Value<QRender::LoadAssetsRequestEvent>());
	}
	else if (event->GetType() == QRender::CreateEntitiesEventType)
	{
		OnCreateEntitiesRequested(event->Value<QRender::CreateEntitiesEvent>());
	}
	else if (event->GetType() == QRender::CreateShadersEventType)
	{
		OnCreateEntitiesRequested(event->Value<QRender::CreateEntitiesEvent>());
	}
	else if (event->GetType() == QRender::DestroyEntitiesEventType)
	{
		OnDestroyEntitiesRequested(event->Value<QRender::DestroyEntitiesEvent>());
	}
	else if (event->GetType() == QRender::UpdateEntitiesEventType)
	{
		OnEntitiesUpdateRequested(event->Value<QRender::UpdateEntitiesEvent>());
	}
	else if (event->GetType() == QRender::UpdateCameraEventType)
	{
		OnCameraUpdated(event->Value<QRender::UpdateCameraEvent>());
	}
	else if (event->GetType() == QRender::CreateRenderComponentEventType)
	{
		OnCreateRenderComponentsRequested(event->Value<QRender::CreateRenderComponentEvent>());
	}
	else if (event->GetType() == QRender::UpdateRenderComponentsType)
	{
		OnUpdateRenderComponentsRequested(event->Value<QRender::UpdateRenderComponentsEvent>());
	}
	else if (event->GetType() == QRender::CreateDirectionalLightEventType)
	{
		OnCreateDirectionalLightRequested(event->Value<QRender::CreateDirectionalLightEvent>());
	}
	else if (event->GetType() == QRender::CreatePointLightEventType)
	{
		OnCreatePointLightRequested(event->Value<QRender::CreatePointLightEvent>());
	}
	else if (event->GetType() == QRender::CreateSpotLightEventType)
	{
		OnCreateSpotLightRequested(event->Value<QRender::CreateSpotLightEvent>());
	}
	else if (event->GetType() == QRender::CreateCameraEventType)
	{
		OnCreateCameraRequested(event->Value<QRender::CreateCameraEvent>());
	}
}

bool RenderLayer::OnWindowResized(const QCore::WindowResizeEvent& windowResizedEvent)
{
	const float w = windowResizedEvent.Width;
	const float h = windowResizedEvent.Height;

	QRender::RenderCommand::SetViewport(0, 0, w, h);
	QRender::RenderCommand::WaitForGPU();

	m_cameraController->Resize(w, h);
	m_editorCamera->Resize(w, h);
	m_renderManager->Resize(w, h);

	return false;
}

void RenderLayer::BuildBatches()
{
	//Execute all three batch filters asynchronously
	auto forwardBatchFuture = std::async(std::launch::async, [&] {FilterBatches(&m_forwardRenderBatchBucket); });
	auto transparentBatchFuture = std::async(std::launch::async, [&] {FilterBatches(&m_transparentRenderBatchBucket); });
	auto shadowBatchFuture = std::async(std::launch::async, [&] {FilterBatches(&m_shadowRenderBatchBucket); });

	//Immediately wait for all three async ops to finish
	forwardBatchFuture.get();
	transparentBatchFuture.get();
	shadowBatchFuture.get(); 

	RenderBucket* renderBuckets[3] = { &m_forwardRenderBatchBucket,&m_transparentRenderBatchBucket,&m_shadowRenderBatchBucket };

	for (int i = 0; i < 3; ++i)
	{
		UploadGPUCommandsToBuffer(renderBuckets[i]);
	}
	
	if (m_forwardRenderBatchBucket.instanceTransforms.size() > 0 && m_forwardRenderBatchBucket.needsTransformRefresh)
	{
		void* bufferAddr = m_forwardRenderBatchBucket.transformStructuredBuffer->Map();
		memcpy(bufferAddr, m_forwardRenderBatchBucket.instanceTransforms.data(), m_forwardRenderBatchBucket.instanceTransforms.size() * sizeof(glm::mat4));
		m_forwardRenderBatchBucket.needsTransformRefresh = false;
	}
}

void RenderLayer::UploadGPUCommandsToBuffer(RenderBucket* bucket)
{

	//GPU ONLY BUFFER
	//Initialize the buffer at the start of the frame if the batch size has changed
	if (bucket->gpuIndirectCommandBuffer->GetCapacitySize() < bucket->batches.size() * sizeof(IndirectArgs))
	{
		QRender::BufferDescriptor bufferDesc = {};
		bufferDesc.bufferAccessType = QRender::BufferAccessType::DEFAULT;
		bufferDesc.bufferType = QRender::BufferUsageType::STORAGE;
		bufferDesc.size = bucket->batches.size() * sizeof(IndirectArgs);
		bufferDesc.structuredBuffer.numElements = bucket->batches.size();
		bufferDesc.structuredBuffer.structuredByteStride = sizeof(IndirectArgs);
		bufferDesc.debugName = L"GPU Indirect Command Buffer";

		//This SHOULD delete the buffer if it's the only one holding the pointer
		bucket->gpuIndirectCommandBuffer.reset();
		bucket->gpuIndirectCommandBuffer = QRender::Buffer::Create(bufferDesc);
	}

	//CPU VISIBLE BUFFER
	if (bucket->needsIndirectRefresh && bucket->batches.size() > 0)
	{
		QRender::BufferDescriptor bufferDesc = {};
		bufferDesc.bufferAccessType = QRender::BufferAccessType::UPLOAD;
		bufferDesc.bufferType = QRender::BufferUsageType::STORAGE;
		bufferDesc.size = sizeof(IndirectArgs) * bucket->flatBatches.size();
		bufferDesc.structuredBuffer.numElements = bucket->flatBatches.size();
		bufferDesc.structuredBuffer.structuredByteStride = sizeof(IndirectArgs);
		bufferDesc.debugName = L"GPU Indirect Command Buffer";

		//Q_RENDER_INFO("Regenerate Indirect buffer");
		Ref<QRender::Buffer> newIndirectBuffer = QRender::Buffer::Create(bufferDesc);

		void* mapAddr = newIndirectBuffer->Map();
		memcpy(mapAddr, bucket->gpuIndirectCommands.data(), bucket->gpuIndirectCommands.size() * sizeof(IndirectArgs));
		newIndirectBuffer->UnMap();

		if (bucket->gpuClearIndirectCommandBuffer != nullptr)
		{
			//move the new one into the old place
			bucket->gpuClearIndirectCommandBuffer.reset();
			bucket->gpuClearIndirectCommandBuffer = std::move(newIndirectBuffer);
			bucket->needsIndirectRefresh = false;
		}
	}
}

void RenderLayer::FilterBatches(RenderBucket* renderPass)
{
	std::vector<uint32_t> newObjects;
	if (renderPass->objectsToDelete.size() > 0)
	{
		//Q_RENDER_INFO("Deleting Objects from Batch");
		//Delete objects from the batch
	}

	//Q_RENDER_INFO("Create object list from scene");
	//Fill the object list that we need to create draw calls from
	newObjects.reserve(renderPass->unbatchedObjects.size());
	for (auto object : renderPass->unbatchedObjects)
	{
		PassObject newObject;
		InstancedRenderable* instancedRenderableData = GetInstancedRenderableViaKey(object.entityID);

		newObject.original = object;
		newObject.modelhash = object.modelId;
		newObject.materialHash = object.materialId;
		newObject.techniqueHash = object.techniqueId;
	
		uint32_t handle = -1;

		//If there are any objects we can reuse in this batch
		if (renderPass->reusableObjects.size() > 0)
		{
			//Grab one that already exists, pop and assign to the objects array
			handle = renderPass->reusableObjects.back().handle;
			renderPass->reusableObjects.pop_back();
			renderPass->objects[handle] = newObject;
		}
		else
		{
			//Push a new object into the object array 
			handle = renderPass->objects.size();
			renderPass->objects.push_back(newObject);
		}

		newObjects.push_back(handle);
	}

	//Clear any unbatched objects, we've batched them now
	renderPass->unbatchedObjects.clear();


	//Q_RENDER_INFO("Creating new draw list");
	std::vector<RenderBatch> newDrawBatches;
	newDrawBatches.reserve(newObjects.size());
	//Fill the draw list with the objects we just assigned
	for (auto object : newObjects)
	{
		RenderBatch newDrawCommand;

		auto obj = renderPass->objects[object];
		newDrawCommand.object.handle = object;
		renderPass->instanceTransforms.push_back(obj.original.transform);
		
		uint64_t materialHash = obj.materialHash;
		uint64_t meshHash = obj.modelhash;
		uint64_t techniqueHash = obj.techniqueHash;
	
		uint32_t combinedTMHash = static_cast<uint32_t>(techniqueHash ^ materialHash);
		uint32_t finalSortHash = uint64_t(combinedTMHash) ^ uint64_t(meshHash);

		//Combine the final sorted hash with a custom key hash incase we want to custom 
		//sort batches somewhere down the line
		newDrawCommand.sortKey = uint64_t(meshHash) | (uint64_t(obj.customKey) << 32);
	
		newDrawBatches.push_back(newDrawCommand);
	}

	//Q_RENDER_INFO("Sorting draws");
	//Sort the draw list by the mesh + material hash we just made, sorting instanced draw calls directly next to each other
	std::sort(newDrawBatches.begin(), newDrawBatches.end(), [](const RenderBatch& A, const RenderBatch& B){
		if (A.sortKey < B.sortKey) { return true; }
		else if (A.sortKey == B.sortKey) { return A.object.handle < B.object.handle; }
		else { return false; }
	});


	//Q_RENDER_INFO("Merge into flat batches");
	//Append and sort the list with existing flat batches
	if (renderPass->flatBatches.size() > 0 && newDrawBatches.size() > 0)
	{
		size_t index = renderPass->flatBatches.size();
		renderPass->flatBatches.reserve(renderPass->flatBatches.size() + newDrawBatches.size());

		for (auto object : newDrawBatches)
		{
			renderPass->flatBatches.push_back(object);
		}

		RenderBatch* begin = renderPass->flatBatches.data();
		RenderBatch* mid = begin + index;
		RenderBatch* end = begin + renderPass->flatBatches.size();

		std::inplace_merge(begin, mid, end, [](const RenderBatch& A, const RenderBatch& B) {
			if (A.sortKey < B.sortKey) { return true; }
			else if (A.sortKey == B.sortKey) { return A.object.handle < B.object.handle; }
			else { return false; }
		});
	}
	//No batches?
	else if (renderPass->flatBatches.size() == 0)
	{
		//Move ownership of the new batches to the flat batches cache
		renderPass->flatBatches = std::move(newDrawBatches);
	}
	//Check for indirect instanced draw calls, merge them if they exist (GPU instancing)
	
	//Clear the previous frames batches and regen draw commands
	renderPass->batches.clear();

	//Regen draw calls, move flat batches into draw calls into batches
	CreateIndirectBatches(renderPass, renderPass->batches, renderPass->flatBatches);

	//Time to flatten the indirect draw calls and merge them if they share a sort index (mesh and material)
	Multibatch newMultiBatch;
	renderPass->multibatches.clear(); 

	newMultiBatch.count = 1;
	newMultiBatch.first = 0;

	//Q_RENDER_INFO("Flatten batches into multi-draw indirect calls (GPU instancing)");
	for (int i = 1; i < renderPass->batches.size(); i++)
	{
		IndirectBatch* joinBatch = &renderPass->batches[newMultiBatch.first];
		IndirectBatch* batch = &renderPass->batches[i];

		//Has the mesh been added to the multi-draw vertex heap?
		
		//TODO: LIAM
		//We will need to do this another time, we don't currently support non-vertex combined meshes
		bool compatibleMesh = true;
		bool sameMat = false;

		if (compatibleMesh && joinBatch->techniqueId == batch->techniqueId && joinBatch->materialId == batch->materialId)
		{
			sameMat = true;
		}

		//TODO: LIAM Add checks for same material and compatible meshes (merged into vertex buffer)
		if (!sameMat || !compatibleMesh)
		{
			renderPass->multibatches.push_back(newMultiBatch);
			newMultiBatch.count = 1;
			newMultiBatch.first = i;
		}
		else {
			newMultiBatch.count++;
		}
	}
	renderPass->multibatches.push_back(newMultiBatch);
}

void RenderLayer::CreateIndirectBatches(RenderBucket* renderPass, std::vector<IndirectBatch>& outBatches, std::vector<RenderBatch>& inObjects)
{
	if (inObjects.size() == 0)
		return;

	//Start the first batch
	IndirectBatch newBatch;
	newBatch.first = 0;
	newBatch.count = 0;

	//Get the first meshID
	newBatch.indirectDrawArgs = renderPass->Get(inObjects[0].object)->original.indirectDrawArgs;
	newBatch.material = renderPass->Get(inObjects[0].object)->original.pbrMaterial;
	newBatch.technique = renderPass->Get(inObjects[0].object)->original.technique;
	newBatch.modelId = renderPass->Get(inObjects[0].object)->modelhash;
	newBatch.techniqueId = renderPass->Get(inObjects[0].object)->materialHash;
	outBatches.push_back(newBatch);

	//This is our comparitive batch. Compare our next batch with the previous batch for similarities
	//in hash values/meshes/materials for GPU instancing
	IndirectBatch* back = &renderPass->batches.back();

	//Q_RENDER_INFO("Create indirect batches");

	//Loop through the objects, and check the next in the list for matches on meshes and materials
	//if both the mesh and the material match, then they can be GPU instanced and later flattened into
	//multi-draws 
	uint64_t lastMaterialHash = renderPass->Get(inObjects[0].object)->materialHash;
	for (int i = 0; i < inObjects.size(); ++i)
	{
		PassObject* obj = renderPass->Get(inObjects[i].object);

		//Check if the meshes are the same via the handle (hash)
		bool sameMesh = obj->modelhash == back->modelId;
		bool sameMaterial = false;

		if (obj->materialHash == lastMaterialHash)
		{
			sameMaterial = true;
		}

		if (!sameMaterial || !sameMesh)
		{
			newBatch.materialId = obj->materialHash;

			if (newBatch.materialId == back->materialId)
			{
				sameMaterial = true;
			}
		}

		//If we have the same mesh and material, GPU instance (increase count to the top batch)
		if (sameMesh && sameMaterial)
		{
			//Increase count, do not push into batches as it is instanced
			back->count++;
		}
		else
		{
			//Initialize the first occurence of this unique draw call and push into our batches list
			newBatch.first = i;
			newBatch.count = 1;
			newBatch.modelId = obj->modelhash;
			newBatch.materialId = obj->materialHash;
			newBatch.techniqueId = obj->techniqueHash;

			outBatches.push_back(newBatch);
			back = &outBatches.back(); //Our new batch we pushed in is the one we want to check against
		}

	}

	CreateGPUCommands(renderPass);
}

void RenderLayer::CreateGPUCommands(RenderBucket* renderPass)
{
	size_t batchSize = renderPass->batches.size();
	renderPass->gpuIndirectCommands.resize(batchSize);

	for (int i = 0; i < batchSize; ++i)
	{
		auto batch = renderPass->batches[i];

		D3D12_DRAW_INDEXED_ARGUMENTS indirectArguments = {};
		indirectArguments.StartInstanceLocation = batch.first; // Mesh base location (either from the mesh or from the batch)
		indirectArguments.StartIndexLocation = batch.indirectDrawArgs->indexStart;//Mesh index start location
		indirectArguments.BaseVertexLocation = batch.indirectDrawArgs->vertexStart;//Mesh vertex start location
		indirectArguments.IndexCountPerInstance = batch.indirectDrawArgs->indexCount; //Mesh index count
		indirectArguments.InstanceCount = batch.count;

		renderPass->gpuIndirectCommands[i].drawIndexedArguments = std::move(indirectArguments);
	}
}

void RenderLayer::OnCreateEntitiesRequested(const QRender::CreateEntitiesEvent& newEntityEvent)
{
	
}

void RenderLayer::OnDestroyEntitiesRequested(const QRender::DestroyEntitiesEvent& removeEvent)
{

}

void RenderLayer::OnEntitiesUpdateRequested(const QRender::UpdateEntitiesEvent& updateEvent)
{
	auto& eids = updateEvent.EntityData.EntityIds;
	auto& aids = updateEvent.EntityData.Assets;
	auto& pids = updateEvent.EntityData.Positions;
	auto& rids = updateEvent.EntityData.Rotations;
	auto& sids = updateEvent.EntityData.Scales;

	/* assume that if the counts for entity ids are not equal to positions, then everything else is wrong too */
	for (size_t k = 0; k < eids.size(); k++)
	{
		InstancedRenderable renderableFound;
		std::map<entt::entity, InstancedRenderable>::iterator it = m_renderables.find(eids[k]);

		if (it != m_renderables.end())
		{
			glm::mat4 newTransform = glm::mat4(1.0f);
			newTransform = glm::translate(newTransform, pids[k]);
			newTransform = glm::scale(newTransform, sids[k]);
			newTransform = glm::transpose(newTransform);

			it->second.transform = newTransform;
		}
	}
}

void RenderLayer::OnCameraUpdated(const QRender::UpdateCameraEvent& cameraData)
{

}

void RenderLayer::OnCreateRenderComponentsRequested(const QRender::CreateRenderComponentEvent& createRenderComponentsEvent)
{
	auto& materials = createRenderComponentsEvent.RenderComponentData.Material;
	auto& models = createRenderComponentsEvent.RenderComponentData.Model;
	auto& techniques = createRenderComponentsEvent.RenderComponentData.Technique;
	auto& entityIds = createRenderComponentsEvent.RenderComponentData.EntityIds;
	auto& positions = createRenderComponentsEvent.RenderComponentData.Positions;
	auto& rotations = createRenderComponentsEvent.RenderComponentData.Rotations;
	auto& scales = createRenderComponentsEvent.RenderComponentData.Scales;
	


	/* assume that if the counts for entity ids are not equal to positions, then everything else is wrong too */
	for (size_t k = 0; k < entityIds.size(); k++)
	{

		bool isTransparent = false;
		glm::mat4 newTransform = glm::mat4(1.0f);
		newTransform = glm::translate(newTransform, positions[k]);
		newTransform = glm::scale(newTransform, scales[k]);
		newTransform = glm::transpose(newTransform);

		Ref<QRender::PBRMaterial> material = GetMaterialViaKey(materials[k]);
		Ref<QRender::Technique> technique = GetGraphicsTechniqueViaKey(techniques[k], models[k]);
		Ref<IndirectDrawArgs> indirectArgs = GetIndirectDrawArgsViaKey(models[k]);
		
		InstancedRenderable newRenderable =
		{
			.entityID = entityIds[k],
			.indirectDrawArgs = indirectArgs,
			.technique = technique,
			.pbrMaterial = material,
			.sortHashkey = 0,
			.transform = newTransform,
		};
		
		std::map<entt::entity, InstancedRenderable>::iterator it = m_renderables.find(entityIds[k]);
		if (it != m_renderables.end())
		{
			//temporary to handle batches
			m_renderables.emplace(std::pair(entityIds[k], newRenderable));
		}
		else
		{
			m_renderables.emplace(std::pair(entityIds[k], newRenderable));

			//Push into the forward, shadowed and transparent buckets (if applicable) 
			if (isTransparent)
			{
				m_transparentRenderBatchBucket.unbatchedObjects.push_back(newRenderable);
			}
			else
			{
				m_forwardRenderBatchBucket.unbatchedObjects.push_back(newRenderable);
				m_forwardRenderBatchBucket.needsTransformRefresh = true;
			}
			m_shadowRenderBatchBucket.unbatchedObjects.push_back(newRenderable);
		}
		//}
	}

}

void RenderLayer::OnUpdateRenderComponentsRequested(const QRender::UpdateRenderComponentsEvent& updateRenderComponentsEvent)
{

}

void RenderLayer::OnCreateDirectionalLightRequested(const QRender::CreateDirectionalLightEvent& createDirectionalLightEvent)
{
	auto& entityIds = createDirectionalLightEvent.DirectionalLightData.EntityIds;
	auto& directionalLightCmp = createDirectionalLightEvent.DirectionalLightData.DirectionalLights;

	/* assume that if the counts for entity ids are not equal to positions, then everything else is wrong too */
	for (size_t k = 0; k < entityIds.size(); k++)
	{
		std::map<entt::entity, InstancedDirectionalLight>::iterator it = m_directionalLights.find(entityIds[k]);
		if (it != m_directionalLights.end())
		{
			
		}
		else
		{
			//new directional light
			InstancedDirectionalLight newDirectionalLight = {};
			newDirectionalLight.color = directionalLightCmp[k].Color;
			newDirectionalLight.direction = directionalLightCmp[k].Direction;

			m_directionalLights.emplace(std::pair(entityIds[k], newDirectionalLight));
		}
	}
}

void RenderLayer::OnCreatePointLightRequested(const QRender::CreatePointLightEvent& createPointLightEvent)
{
	auto& entityIds = createPointLightEvent.PointLightData.EntityIds;
	auto& pointLightCmp = createPointLightEvent.PointLightData.PointLights;
	auto& transformCmp = createPointLightEvent.PointLightData.Transforms;

	/* assume that if the counts for entity ids are not equal to positions, then everything else is wrong too */
	for (size_t k = 0; k < entityIds.size(); k++)
	{
		std::map<entt::entity, InstancedPointLight>::iterator it = m_pointLights.find(entityIds[k]);
		if (it != m_pointLights.end())
		{
	
		}
		else
		{
			//New point light
			InstancedPointLight newPointLight = {};
			newPointLight.position = Vector4f(transformCmp[k].Position, 1.0f);
			newPointLight.color = pointLightCmp[k].Color;
			newPointLight.constant = pointLightCmp[k].Constant;
			newPointLight.linear = pointLightCmp[k].Linear;
			newPointLight.exponential = pointLightCmp[k].Exp;
			m_pointLights.emplace(std::pair(entityIds[k], newPointLight));
		}
	}
}

void RenderLayer::OnCreateSpotLightRequested(const QRender::CreateSpotLightEvent& createSpotLightEvent)
{
	auto& entityIds = createSpotLightEvent.SpotLightData.EntityIds;
	auto& spotLightCmp = createSpotLightEvent.SpotLightData.SpotLights;
	auto& transformCmp = createSpotLightEvent.SpotLightData.Transforms;

	/* assume that if the counts for entity ids are not equal to positions, then everything else is wrong too */
	for (size_t k = 0; k < entityIds.size(); k++)
	{
		std::map<entt::entity, InstancedSpotLight>::iterator it = m_spotLights.find(entityIds[k]);
		if (it != m_spotLights.end())
		{
			
		}
		else
		{
			//new directional light
			InstancedSpotLight newSpotLight = {};
			newSpotLight.color = spotLightCmp[k].Color;
			newSpotLight.position = Vector4f(transformCmp[k].Position, 1.0f);
			newSpotLight.direction = spotLightCmp[k].Direction;
			newSpotLight.cutoff = spotLightCmp[k].Cutoff;

			m_spotLights.emplace(std::pair(entityIds[k], newSpotLight));
		}
	}
}

void RenderLayer::OnCreateCameraRequested(const QRender::CreateCameraEvent& createCameraEvent)
{
	auto& entityIds = createCameraEvent.CameraData.EntityIds;
	auto& cameraCmp = createCameraEvent.CameraData.CameraComponent;
	auto& transformCmp = createCameraEvent.CameraData.Transforms;

	/* assume that if the counts for entity ids are not equal to positions, then everything else is wrong too */
	for (size_t k = 0; k < entityIds.size(); k++)
	{
		std::map<entt::entity, QRender::SceneCamera>::iterator it = m_sceneCameras.find(entityIds[k]);
		if (it != m_sceneCameras.end())
		{
		

		}
		else
		{
			//New camera
			InstancedCamera newCamera = {};
			newCamera.position = Vector4f(transformCmp[k].Position, 1.0f);
			newCamera.orthographic = cameraCmp[k].Orthographic;
			newCamera.zoom = cameraCmp[k].Zoom;
			newCamera.nearClip = cameraCmp[k].NearClip;
			newCamera.farClip = cameraCmp[k].FarClip;
			newCamera.fov = cameraCmp[k].FOV;
			newCamera.viewportXY = cameraCmp[k].viewportXY;
			newCamera.viewportWH = cameraCmp[k].viewportWH;
			newCamera.fov = cameraCmp[k].FOV;

			QRender::SceneCamera newSceneCamera = QRender::SceneCamera(newCamera);

			m_sceneCameras.emplace(std::pair(entityIds[k], newSceneCamera));
		}
	}
}

void RenderLayer::OnLoadAssetsRequest(const QRender::LoadAssetsRequestEvent& updateEvent)
{

}

void RenderLayer::CommitPendingGPUCopyRequests()
{

}

void RenderLayer::CommitPerFrameData()
{
	CommitTransformData();
}

void RenderLayer::CommitTransformData()
{

}

InstancedRenderable* RenderLayer::GetInstancedRenderableViaKey(entt::entity key)
{
	std::map<entt::entity, InstancedRenderable>::iterator it = m_renderables.find(key);
	if (it != m_renderables.end())
	{
		return &it->second;
	}
	else
	{
		Q_RENDER_ERROR("INVALID KEY: BATCH LEAK FAILURE");
		assert(true);
		return nullptr;
	}
}

Ref<QRender::Technique> RenderLayer::GetGraphicsTechniqueViaKey(AssetMetadata key, AssetMetadata modelKey)
{
	std::map<AssetMetadata, Ref<QRender::Technique>>::iterator it = m_renderTechniques.find(key);
	if (it != m_renderTechniques.end())
	{
		//return existing one
		return it->second;
	}
	else
	{
		//This is describing the techniques layout
		QRender::DirectXPipelineLayout pipelineLayout = {};
		pipelineLayout.AddConstantResourceView("camera matrices", 0, QRender::ShaderVisibility::all); //0
		pipelineLayout.AddDescriptorTable("gbuffer", 0, 7, 0, QRender::DescriptorTableType::srv, QRender::ShaderVisibility::all); // 1
		pipelineLayout.AddDescriptorTable("Bindless Textures", 8, -1, 1, QRender::DescriptorTableType::srv, QRender::ShaderVisibility::all); // 2
		pipelineLayout.AddShaderResourceView("Model Transforms", 7, QRender::ShaderVisibility::all); //3 
		pipelineLayout.AddConstantResourceView("Skinned matrices data", 1, QRender::ShaderVisibility::all);// 4
		pipelineLayout.AddConstantResourceView("Material data", 2, QRender::ShaderVisibility::all);// 5
		pipelineLayout.AddConstantResourceView("Directional Light", 3, QRender::ShaderVisibility::all); // 6
		pipelineLayout.AddConstantResourceView("debug shader values", 4, QRender::ShaderVisibility::all);// 7
		pipelineLayout.AddSampler("Point Sampler", 0, QRender::SamplerType::min_mag_mip_linear);
		pipelineLayout.AddSampler("Anisotropic Sampler", 1, QRender::SamplerType::anisotropic);
		pipelineLayout.CreateRootSignature();

		//Create new render technique
		Ref<TechniqueAsset> techniqueAsset = m_assetDatabase.RequestAsset<TechniqueAsset>(key);
		Ref<TechniqueMetadata> techniqueMetadata = techniqueAsset->GetRef();
		
		//Create new model args and add them to the big scene vertex and index buffer
		Ref<ModelAsset> modelAsset = m_assetDatabase.RequestAsset<ModelAsset>(modelKey);
		while (!modelAsset->IsLoaded())
		{

		}
		Ref<ModelMetadata> modelMetadata = modelAsset->GetRef();

		//Grab the shaders from the asset database
		Ref<ShaderAsset> vertShader = m_assetDatabase.RequestAsset<ShaderAsset>(techniqueMetadata->GetVertexShaderAsset());
		Ref<ShaderAsset> pixelShader = m_assetDatabase.RequestAsset<ShaderAsset>(techniqueMetadata->GetPixelShaderAsset());

		while (!vertShader->IsLoaded());
		while (!pixelShader->IsLoaded());

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = _countof(QRender::DirectXVertexInputLayouts::kVertexInputStaticMesh);
		inputLayoutDesc.pInputElementDescs = QRender::DirectXVertexInputLayouts::kVertexInputStaticMesh;

		QRender::PipelineCreateInfo pipelineCreateInfo = {};

		D3D12_SHADER_BYTECODE* vertBytecode = static_cast<D3D12_SHADER_BYTECODE*>(vertShader->GetRef()->GetShaderBytecode());
		D3D12_SHADER_BYTECODE* fragBytecode = static_cast<D3D12_SHADER_BYTECODE*>(pixelShader->GetRef()->GetShaderBytecode());

		///Shader specific
		pipelineCreateInfo.depthTestEnabled = techniqueMetadata->GetDepthWriteEnabled();
		pipelineCreateInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineCreateInfo.vertShader = *vertBytecode;
		pipelineCreateInfo.pixelShader = *fragBytecode;
		pipelineCreateInfo.pipelineLayout = pipelineLayout; //GLOBAL - GET FORWARD PIPELINE LAYOUT
		pipelineCreateInfo.rtvFormats.push_back(DXGI_FORMAT_R8G8B8A8_UNORM);//TODO - Store this somewhere as a ref

		//Model Specific
		pipelineCreateInfo.vertexInputLayout = inputLayoutDesc;
		pipelineCreateInfo.wireframeModeEnabled = modelMetadata->GetWireframeEnabled();
		pipelineCreateInfo.cullMode = modelMetadata->GetCullingMode();
	
		Ref<QRender::Technique> newTechnique = QRender::Technique::Create(pipelineCreateInfo);
		m_renderTechniques.emplace(std::pair(key, newTechnique));
		return newTechnique;
	}
}

Ref<QRender::PBRMaterial> RenderLayer::GetMaterialViaKey(AssetMetadata key)
{
	std::map<AssetMetadata, Ref<QRender::PBRMaterial>>::iterator it = m_renderMaterials.find(key);
	if (it != m_renderMaterials.end())
	{
		//return existing one
		return it->second;
	}
	else
	{
		//Create new material
		Ref<MaterialAsset> materialAsset = m_assetDatabase.RequestAsset<MaterialAsset>(key);

		Ref<QRender::PBRMaterial> pbrMaterial = static_pointer_cast<QRender::PBRMaterial>(materialAsset->GetRef());
		m_renderMaterials.emplace(std::pair(key, pbrMaterial));
		return pbrMaterial;
	}
}

Ref<IndirectDrawArgs> RenderLayer::GetIndirectDrawArgsViaKey(AssetMetadata key)
{
	std::map<AssetMetadata, Ref<IndirectDrawArgs>>::iterator it = m_renderModels.find(key);
	if (it != m_renderModels.end())
	{
		//return existing one
		return it->second;
	}
	else
	{
		//Create new model args and add them to the big scene vertex and index buffer
		Ref<ModelAsset> modelAsset = m_assetDatabase.RequestAsset<ModelAsset>(key);
		Ref<ModelMetadata> modelMetadata = modelAsset->GetRef();
		QRender::StaticMesh* mesh = modelMetadata->GetStaticMeshPtr();

		uint32_t indexCount = mesh->indexCount;
		uint32_t vertexCount = mesh->vertexCount;

		Ref<IndirectDrawArgs> indirectArguments = MakeRef<IndirectDrawArgs>();
		indirectArguments->indexCount = indexCount;//Mesh index start location
		indirectArguments->vertexCount = vertexCount;//Mesh vertex start location
		indirectArguments->indexStart = m_currentIndexCount; //Mesh index count
		indirectArguments->vertexStart = m_currentVertexCount;

		//Combine into big vertex buffer, return the offsets
		m_currentIndexCount += indexCount;
		m_currentVertexCount += vertexCount;
		
		//Merge it into the scene buffer since it's a unique mesh
		m_forwardRenderBatchBucket.sceneCombinedIndexBuffer->AppendData(mesh->indices.size() * sizeof(uint32_t), reinterpret_cast<void*>(mesh->indices.data()));
		m_forwardRenderBatchBucket.sceneCombinedVertexBuffer->AppendData(mesh->vertices.size() * sizeof(QRender::PrimitiveVertex), reinterpret_cast<void*>(mesh->vertices.data()));
		
		m_renderModels.emplace(std::pair(key, indirectArguments));
		return indirectArguments;
	}
}

Ref<IndirectDrawArgs> RenderLayer::CreateIndirectDrawArgs(const QRender::StaticMesh& mesh)
{
	return Ref<IndirectDrawArgs>();
}


PassObject* RenderBucket::Get(Handle<PassObject> object)
{
	return &objects[object.handle];
}
