#include "vulkan_command_buffer.h"
#include "command_buffer.h"
#include "vulkan_graphics_context.h"

namespace KNR
{
	CommandBuffer* CommandBuffer::Create(CommandBufferType type)
	{
		return new VulkanCommandBuffer(type);
	}

	VulkanCommandBuffer::VulkanCommandBuffer(CommandBufferType type)
	{
		UNREFERENCED_PARAMETER(type);

		m_workToBeSubmitted = true;
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		
	}

	void VulkanCommandBuffer::AddToCommandCallbackList(Delegate<>::Func_type&& func)
	{
		m_commandBufferCallbackList += std::forward<Delegate<>::Func_type>(func);
	}

}


	


