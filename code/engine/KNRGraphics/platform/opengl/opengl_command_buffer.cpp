#include "opengl_command_buffer.h"
#include "opengl_graphics_context.h"

namespace KNR
{
	CommandBuffer* CommandBuffer::Create(CommandBufferType type)
	{
		return new OpenglCommandBuffer(type);
	}

	OpenglCommandBuffer::OpenglCommandBuffer(CommandBufferType type)
	{
		UNREFERENCED_PARAMETER(type);

		m_workToBeSubmitted = true;
	}

	OpenglCommandBuffer::~OpenglCommandBuffer()
	{
		
	}

	void OpenglCommandBuffer::AddToCommandCallbackList(Delegate<>::Func_type&& func)
	{
		m_commandBufferCallbackList += std::forward<Delegate<>::Func_type>(func);
	}

}


	


