#include "directx11_command_buffer.h"
#include "directx11_graphics_context.h"


namespace KNR
{
	CommandBuffer* CommandBuffer::Create(CommandBufferType type)
	{
		return new DirectX11CommandBuffer(type);
	}

	DirectX11CommandBuffer::DirectX11CommandBuffer(CommandBufferType type)
	{
		UNREFERENCED_PARAMETER(type);

		m_workToBeSubmitted = true;
	}

	DirectX11CommandBuffer::~DirectX11CommandBuffer()
	{
		
	}

	void DirectX11CommandBuffer::AddToCommandCallbackList(Delegate<>::Func_type&& func)
	{
		m_commandBufferCallbackList += std::forward<Delegate<>::Func_type>(func);
	}

}


	


