#pragma once
#include "render_types.h"

namespace KNR
{
	class CommandBuffer
	{
	public:
		~CommandBuffer() = default;

		inline CommandBufferType GetType() { return m_type; }

		static CommandBuffer* Create(const CommandBufferType type);

	protected:
		CommandBufferType m_type;
	};
}