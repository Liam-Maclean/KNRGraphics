#include "qlpch.h"
#include "screen_quad.h"


namespace QRender
{
	ScreenQuad::ScreenQuad()
	{
		m_quadVertices.push_back(QuadVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
		m_quadVertices.push_back(QuadVertex(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f));
		m_quadVertices.push_back(QuadVertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		m_quadVertices.push_back(QuadVertex(1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f));

		BufferDescriptor vertexBufferDesc = {};
		vertexBufferDesc.bufferAccessType = BufferAccessType::DEFAULT;
		vertexBufferDesc.bufferType = BufferUsageType::VERTEX;
		vertexBufferDesc.vertexBuffer.stride = sizeof(QRender::PrimitiveVertex);
		vertexBufferDesc.size = sizeof(QuadVertex) * m_quadVertices.size();
		vertexBufferDesc.debugName = L"Screen Quad Vertex Buffer";
		vertexBufferDesc.initialData = m_quadVertices.data();

		m_vertexBuffer = Buffer::Create(vertexBufferDesc);
	}

	ScreenQuad::~ScreenQuad()
	{
	}

	void ScreenQuad::Draw(Ref<DirectXCommandBuffer> commandList)
	{
		commandList->Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_vertexBuffer->Bind(commandList);
		commandList->Get()->DrawInstanced(4, 1, 0, 0);
	}
}
