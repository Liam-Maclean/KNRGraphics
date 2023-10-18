#pragma once
#include "buffer.h"
#include "vertex.h"
#include "command_buffer.h"

namespace KNR
{
	class DirectXCommandList;
	class ScreenQuad
	{
	public:
		ScreenQuad();
		~ScreenQuad();
		void Draw(CommandBuffer* commandList);

	private:
		Buffer* m_vertexBuffer;
	};
}