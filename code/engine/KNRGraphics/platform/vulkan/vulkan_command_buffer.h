#pragma once
#include "command_buffer.h"
#include "vulkan_fence.h"
#include "vulkan/vulkan.h"
#include "delegate/callback.h"

namespace KNR
{
	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(CommandBufferType type);
		~VulkanCommandBuffer();

	private:
		void AddToCommandCallbackList(Delegate<>::Func_type&& func);

		CommandBufferType GetType() { return m_type; }
		bool GetWorkToBeSubmitted() { return m_workToBeSubmitted; }
		inline VkCommandBuffer Get() { return m_commandList; }; //Liam - We shouldn't really be using this but it's a LOT of work to 
																		   //handle all the cases where we want to replace the old command buffer all
																		   //at once and keep it working, I'll come back to remove direct access to the buffer later
	private:
		VkCommandBuffer m_commandList;
		VkCommandPool m_commandPool;
		VulkanFence* m_fence;
		CommandBufferType m_type;
		Delegate<> m_commandBufferCallbackList;

		bool m_workToBeSubmitted;
		
	};


}