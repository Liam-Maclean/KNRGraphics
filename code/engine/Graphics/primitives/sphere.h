#pragma once
#include "core/core.h"
#include "graphics/buffer.h"
#include "graphics/material.h"
#include "graphics/vertex.h"

namespace QRender
{
	class DirectXCommandBuffer;
	class Sphere
	{
	public:
		Sphere();
		~Sphere();

		void Update();
		void Draw(Ref<DirectXCommandBuffer> commandList);
	protected:

	private:
		Ref <Buffer> m_vertexBuffer;
		Ref <Buffer> m_indexBuffer;
		QRender::PBRMaterial m_material;

		size_t m_indexCount;
		size_t m_vertexCount;
		std::vector < PrimitiveVertex > m_vertices;
		std::vector < uint32_t > m_indices;
	};
}
