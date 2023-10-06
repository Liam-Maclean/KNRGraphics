#pragma once
#include "assets/asset.h"
#include <d3d12.h>
#include "buffer.h"
namespace QRender
{
	class UniformBuffer;
	class VertexBuffer;
	class IndexBuffer;
	class Technique;
	class PBRMaterial;
}


struct IndirectDrawArgs
{
	IndirectDrawArgs() {
		vertexStart = 0;
		indexStart = 0;
		indexCount = 0;
		vertexCount = 0;
	};
	uint32_t vertexStart;
	uint32_t indexStart;
	uint32_t indexCount;
	uint32_t vertexCount;
};
//The smallest structure we can use to sort renderable data
// EntityID - Debugging
// Mesh - To access the mesh data if N of meshes rendered is 0
// MeshAssetID - To fast sort draw calls based on the mesh used
// Transparency - Opaque or transparent (deffered or forward)
// TransformIdx - Push constant value of the bindless transform array
//
struct InstancedRenderable
{
	entt::entity entityID;
	Ref<IndirectDrawArgs> indirectDrawArgs;
	Ref<QRender::Technique> technique;
	Ref<QRender::PBRMaterial> pbrMaterial;
	uint64_t techniqueId;
	uint64_t modelId;
	uint64_t materialId;
	uint64_t sortHashkey;
	glm::mat4 transform; //transforms
};

struct InstancedDirectionalLight
{
	glm::vec4 color;
	glm::vec4 direction;
	glm::vec4 lightPos;
};

struct InstancedPointLight
{
	glm::vec4 position;
	glm::vec4 color;
	float radius;
	float constant;
	float linear;
	float exponential;
};

struct InstancedSpotLight
{
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 color;
	float cutoff;
};

struct InstancedCamera
{
	glm::vec4 position;
	bool orthographic;
	float zoom;
	float nearClip;
	float farClip;
	float fov;
	glm::vec2 viewportXY;
	glm::vec2 viewportWH;
};

struct InstancedProbe
{
	uint32_t transformIdx;
};


template<typename T>
struct Handle {
	uint32_t handle;
};

enum class MeshRenderPassType
{
	Forward,
	Shadows,
	Transparency
};

struct IndirectArgs
{
	D3D12_DRAW_INDEXED_ARGUMENTS drawIndexedArguments;
};

struct PassObject {
	//Material material;
	uint64_t modelhash;
	uint64_t techniqueHash;
	uint64_t materialHash;
	Handle<uint32_t> meshID;
	InstancedRenderable original;
	int32_t builtbatch;
	uint32_t customKey;
};

struct RenderBatch {
	Handle<PassObject> object;
	uint64_t sortKey;

	bool operator==(const RenderBatch& other) const
	{
		return object.handle == other.object.handle && sortKey == other.sortKey;
	}
};

struct IndirectBatch {

	Ref<IndirectDrawArgs> indirectDrawArgs;
	Ref<QRender::PBRMaterial> material;
	Ref<QRender::Technique> technique;
	uint64_t modelId;
	uint64_t materialId;
	uint64_t techniqueId;
	uint32_t first;
	uint32_t count;
};

struct GPUInstance {
	uint32_t batchID;
	uint32_t objectID;
};

struct Multibatch {
	uint32_t first;
	uint32_t count;
};


struct RenderBucket
{
	//Final draw indirect parts
	std::vector<Multibatch> multibatches;

	//Draw indirect batches
	std::vector<IndirectBatch> batches;

	//Sorted list of objects (by hash)
	//This is ALL draw calls without instancing (hence flat)
	//If we wish to disable instancing, use flat batches as the final draw
	std::vector<RenderBatch> flatBatches;

	//Unsorted and unbatched renderables that need to be appended to flat batches
	std::vector<InstancedRenderable> unbatchedObjects;

	//unsorted objects
	std::vector<PassObject> objects;

	//Resuable objects 
	std::vector<Handle<PassObject>> reusableObjects;

	//objects pending to be removed from the batch list
	std::vector<Handle<PassObject>> objectsToDelete;

	std::vector<IndirectArgs> gpuIndirectCommands;

	Ref<QRender::Buffer> gpuIndirectCommandBuffer;     //Our juicy draw commands

	Ref<QRender::Buffer> gpuClearIndirectCommandBuffer; //We use this one to write commands (CPU visibile) and copy over to the GPU only uniform buffer

	Ref<QRender::Buffer> transformStructuredBuffer; //transform buffer

	Ref<QRender::Buffer> compactedInstanceBuffer; //compacted instance buffer (our output buffer from compute)

	Ref<QRender::Buffer> passObjectsBuffer; //our GPU instances

	Ref<QRender::Buffer> sceneCombinedVertexBuffer;
	
	Ref<QRender::Buffer> sceneCombinedIndexBuffer;

	std::vector<glm::mat4> instanceTransforms;

	MeshRenderPassType type;

	PassObject* Get(Handle<PassObject> object);

	void Init()
	{
		//Pre allocate 150000 transforms into the forwards structure buffer
		QRender::BufferDescriptor indirectCommandDescriptor = {};
		indirectCommandDescriptor.bufferAccessType = QRender::BufferAccessType::DEFAULT;
		indirectCommandDescriptor.bufferType = QRender::BufferUsageType::STORAGE;
		indirectCommandDescriptor.size = sizeof(IndirectArgs);
		indirectCommandDescriptor.structuredBuffer.numElements = 1;
		indirectCommandDescriptor.structuredBuffer.structuredByteStride = sizeof(IndirectArgs);
		indirectCommandDescriptor.debugName = L"GPU Indirect Commands";

		//Pre allocate 150000 transforms into the forwards structure buffer
		QRender::BufferDescriptor clearIndirectCommandBuffer = {};
		clearIndirectCommandBuffer.bufferAccessType = QRender::BufferAccessType::DEFAULT;
		clearIndirectCommandBuffer.bufferType = QRender::BufferUsageType::STORAGE;
		clearIndirectCommandBuffer.size = sizeof(IndirectArgs);
		clearIndirectCommandBuffer.structuredBuffer.numElements = 1;
		clearIndirectCommandBuffer.structuredBuffer.structuredByteStride = sizeof(IndirectArgs);
		clearIndirectCommandBuffer.debugName = L"GPU Clear Indirect Commands";
		//Initialize the buffers with SOME data
		gpuIndirectCommandBuffer = QRender::Buffer::Create(indirectCommandDescriptor);
		gpuClearIndirectCommandBuffer = QRender::Buffer::Create(clearIndirectCommandBuffer);
	}

	bool needsIndirectRefresh = true;
	bool needsInstanceRefresh = true;
	bool needsTransformRefresh = true;
};

