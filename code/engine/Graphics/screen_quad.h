#pragma once
#include "buffer.h"
#include "vertex.h"
#include "platform/directx/directx_graphics_context.h"

namespace KNR
{
	class DirectXCommandList;
	class ScreenQuad
	{
	public:
		ScreenQuad();
		~ScreenQuad();
		void Draw(DirectXCommandBuffer* commandList);

	private:
		Buffer* m_vertexBuffer;
		std::vector< QuadVertex> m_quadVertices;
	};
}