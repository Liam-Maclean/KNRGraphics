#include <qlpch.h>
#include "plane.h"
#include "graphics/platform/directx/directx_graphics_context.h"

namespace QRender
{

	Plane::Plane()
	{
		CreatePlaneGrid();

		m_indexCount = m_indices.size();
		m_vertexCount = m_vertices.size();

		size_t vertexBufferSize = m_vertexCount * sizeof(PrimitiveVertex);
		size_t indexBufferSize = m_indexCount * sizeof(uint32_t);

		BufferDescriptor vertexBufferDesc = {};
		vertexBufferDesc.bufferAccessType = BufferAccessType::DEFAULT;
		vertexBufferDesc.bufferType = BufferUsageType::VERTEX;
		vertexBufferDesc.vertexBuffer.stride = sizeof(QRender::PrimitiveVertex);
		vertexBufferDesc.size = vertexBufferSize;
		vertexBufferDesc.debugName = L"Plane Vertex Buffer";
		vertexBufferDesc.initialData = m_vertices.data();

		BufferDescriptor indexBufferDesc = {};
		indexBufferDesc.bufferAccessType = BufferAccessType::DEFAULT;
		indexBufferDesc.bufferType = BufferUsageType::INDEX;
		indexBufferDesc.size = indexBufferSize;
		indexBufferDesc.debugName = L"Plane Index Buffer";
		indexBufferDesc.initialData = m_indices.data();

		m_vertexBuffer = Buffer::Create(vertexBufferDesc);
		m_indexBuffer = Buffer::Create(indexBufferDesc);
	}

	Plane::~Plane()
	{

	}

	float Plane::GetRandomFloat(float randMin, float randMax)
	{
		float range = randMax - randMin;
		float random = range * ((((float)rand()) / (float)RAND_MAX)) + randMin;
		return random;
	}

	void Plane::CreatePlaneGrid()
	{
		//Do a minimum 3x3 grid of planes and duplicate those 3x3 grids
		PrimitiveVertex vertex;
		for (int row = 0; row < m_gridHeight; ++row) {
			for (int col = 0; col < m_gridWidth; ++col) {
				vertex.Position.x = (float)col;
				vertex.Position.y = 0.0f;
				vertex.Position.z = (float)row;
				vertex.Normal = glm::vec4(0.0, 1.0f, 0.0f, 1.0f);
				vertex.TexCoord = glm::vec2(0.0, 1.0f);
				m_vertices.push_back(vertex);

				//Only push indicies if we aren't on the final row since we've already done those.
				if (row != (m_gridHeight - 1) && col != (m_gridWidth - 1))
				{
					m_indices.push_back(row * m_gridHeight + col);
					m_indices.push_back(row * m_gridHeight + (col + 1));
					m_indices.push_back((row + 1) * m_gridHeight + col);

					m_indices.push_back((row + 1) * m_gridHeight + col);
					m_indices.push_back((row * m_gridHeight + (col + 1)));
					m_indices.push_back((row + 1) * m_gridHeight + (col + 1));
				}
			}
		}
	}

	void Plane::Update()
	{

	}

	void Plane::Draw(Ref<DirectXCommandBuffer> commandList)
	{
		commandList->Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_vertexBuffer->Bind(commandList);
		m_indexBuffer->Bind(commandList);
		commandList->Get()->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
	}

}


