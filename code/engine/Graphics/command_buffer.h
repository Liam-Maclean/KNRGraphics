#pragma once
#include "render_types.h"

namespace KNR
{
	class CommandBuffer
	{
	public:
		~CommandBuffer();

		static CommandBuffer* Create(const CommandBufferType type);
	};
}