#include <qlpch.h>
#include "cube.h"
#include "graphics/platform/directx/directx_graphics_context.h"

namespace QRender
{
	Cube::Cube()
	{
		m_vertices = {
			//Front face
			{	glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec4(0.0f ,0.0f, 0.0f, 0.0f)},
			{	glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(0.5f, -0.5f, 0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},

			//Back Face
			{	glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec4(0.0f ,0.0f, 0.0f, 0.0f)},
			{	glm::vec4(0.5f, 0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},

			//left
			{	glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec4(0.0f ,0.0f, 0.0f, 0.0f)},
			{	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(-1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},

			//front
			{	glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec4(0.0f ,0.0f, 0.0f, 0.0f)},
			{	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(-1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},

			//back
			{	glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec4(0.0f ,0.0f, 0.0f, 0.0f)},
			{	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(-1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			//bottom
			{	glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec4(0.0f ,0.0f, 0.0f, 0.0f)},
			{	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(-1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
			{	glm::vec4(1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f)},
		};



		m_indexCount = m_indices.size();
		m_vertexCount = m_vertices.size();
		size_t vertexBufferSize = m_vertexCount * sizeof(QRender::PrimitiveVertex);
		size_t indexBufferSize = m_indexCount * sizeof(uint32_t);

		BufferDescriptor vertexBufferDesc = {};
		vertexBufferDesc.bufferAccessType = BufferAccessType::DEFAULT;
		vertexBufferDesc.bufferType = BufferUsageType::VERTEX;
		vertexBufferDesc.vertexBuffer.stride = sizeof(QRender::PrimitiveVertex);
		vertexBufferDesc.size = vertexBufferSize;
		vertexBufferDesc.debugName = L"Cube Vertex Buffer";
		vertexBufferDesc.initialData = m_vertices.data();

		BufferDescriptor indexBufferDesc = {};
		indexBufferDesc.bufferAccessType = BufferAccessType::DEFAULT;
		indexBufferDesc.bufferType = BufferUsageType::INDEX;
		indexBufferDesc.size = indexBufferSize;
		indexBufferDesc.debugName = L"Cube Index Buffer";
		indexBufferDesc.initialData = m_indices.data();

		m_vertexBuffer = QRender::Buffer::Create(vertexBufferDesc);
		m_indexBuffer = QRender::Buffer::Create(indexBufferDesc);
	}

	Cube::~Cube()
	{
		m_vertexBuffer.reset();
		m_vertexBuffer = 0;

		m_indexBuffer.reset();
		m_indexBuffer = 0;
	}

	void Cube::Update()
	{

	}

	void Cube::Draw(Ref<DirectXCommandBuffer> commandList)
	{
		commandList->Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_vertexBuffer->Bind(commandList);
		m_indexBuffer->Bind(commandList);
		commandList->Get()->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
	}
}
