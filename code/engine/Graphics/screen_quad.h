#pragma once
#include "graphics/buffer.h"
#include "graphics/vertex.h"
#include "graphics/platform/directx/directx_graphics_context.h"

namespace QRender
{
	class DirectXCommandList;
	class ScreenQuad
	{
	public:
		ScreenQuad();
		~ScreenQuad();
		void Draw(Ref<DirectXCommandBuffer> commandList);

	private:
		Ref<Buffer> m_vertexBuffer;
		std::vector< QuadVertex> m_quadVertices;

	};

}