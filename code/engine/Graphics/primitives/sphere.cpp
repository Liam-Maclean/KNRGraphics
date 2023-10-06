#include <qlpch.h>
#include "sphere.h"
#include "graphics/platform/directx/directx_graphics_context.h"

namespace QRender
{
	Sphere::Sphere()
	{
		m_vertices = {
			{},
			{},
			{},
			{},

			{},
			{},
			{},
			{},
		};

		m_indexCount = m_indices.size();
		m_vertexCount = m_vertices.size();

		size_t vertexBufferSize = m_vertexCount * sizeof(PrimitiveVertex);
		size_t indexBufferSize = m_indexCount * sizeof(uint32_t);

		BufferDescriptor vertexBufferDesc = {};
		vertexBufferDesc.bufferAccessType = BufferAccessType::DEFAULT;
		vertexBufferDesc.bufferType = BufferUsageType::VERTEX;
		vertexBufferDesc.vertexBuffer.stride = sizeof(QRender::PrimitiveVertex);
		vertexBufferDesc.size = vertexBufferSize;
		vertexBufferDesc.debugName = L"Sphere Vertex Buffer";
		vertexBufferDesc.initialData = m_vertices.data();

		BufferDescriptor indexBufferDesc = {};
		indexBufferDesc.bufferAccessType = BufferAccessType::DEFAULT;
		indexBufferDesc.bufferType = BufferUsageType::INDEX;
		indexBufferDesc.size = indexBufferSize;
		indexBufferDesc.debugName = L"Sphere Index Buffer";
		indexBufferDesc.initialData = m_indices.data();

		m_vertexBuffer = Buffer::Create(vertexBufferDesc);
		m_indexBuffer = Buffer::Create(indexBufferDesc);

	}

	Sphere::~Sphere()
	{

	}

	void Sphere::Update()
	{
	}

	void Sphere::Draw(Ref<DirectXCommandBuffer> commandList)
	{
		commandList->Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_vertexBuffer->Bind(commandList);
		m_indexBuffer->Bind(commandList);
		commandList->Get()->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
	}
}