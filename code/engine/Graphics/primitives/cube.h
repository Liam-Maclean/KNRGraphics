#pragma once
#include "core/core.h"
#include "graphics/buffer.h"
#include "graphics/material.h"



namespace QRender
{
	class DirectXCommandBuffer;
	class Cube
	{
	public:
		Cube();
		~Cube();

		void Update();
		void Draw(Ref<DirectXCommandBuffer> commandList);
		Ref<Buffer> GetVertexBuffer() { return m_vertexBuffer; }
		Ref<Buffer> GetIndexBuffer() { return m_indexBuffer; }
	protected:

	private:
		Ref <Buffer> m_vertexBuffer;
		Ref <Buffer> m_indexBuffer;
		PBRMaterial m_material;

		size_t m_indexCount;
		size_t m_vertexCount;
		std::vector < PrimitiveVertex > m_vertices;
		std::vector < uint32_t > m_indices;
	};
}