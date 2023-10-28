#pragma once
#include "command_buffer.h"
#include "opengl_fence.h"
#include "delegate/callback.h"

namespace KNR
{
	class OpenglCommandBuffer : public CommandBuffer
	{
	public:
		OpenglCommandBuffer(CommandBufferType type);
		~OpenglCommandBuffer();

	private:
		void AddToCommandCallbackList(Delegate<>::Func_type&& func);
		CommandBufferType GetType() { return m_type; }
		bool GetWorkToBeSubmitted() { return m_workToBeSubmitted; }

	private:
		CommandBufferType m_type;
		Delegate<> m_commandBufferCallbackList;

		bool m_workToBeSubmitted;	
	};
}