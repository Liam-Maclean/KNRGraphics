#pragma once
#include "platform/vulkan/vulkan_fence.h"
#include "vulkan/vulkan.h"
namespace KNR
{
	enum class CommandBufferType
	{
		copy,
		compute,
		graphics,
	};
	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(CommandBufferType type);
		~VulkanCommandBuffer();

		void Reset();
		void Reset(VkCommandPool customPool);
		void Close();
		void Submit(VkQueue queue);
		void SubmitWorkImmediate();
		void Wait();
		void UpdateCopyResource(VkBuffer* dstResource, VkBuffer* srcResource,  std::vector<D3D12_SUBRESOURCE_DATA> subresources);
		void UpdateCopyResource(VkBuffer* dstResource, VkBuffer* srcResource, D3D12_SUBRESOURCE_DATA subresources);
		void CopyResource(VkBuffer* dstResource, VkBuffer* srcResource);
		void AddToCommandCallbackList(QCore::Delegate<>::Func_type&& func);

		CommandBufferType GetType() { return m_type; }
		bool GetWorkToBeSubmitted() { return m_workToBeSubmitted; }
		inline VkCommandBuffer Get() { return m_commandList; }; //Liam - We shouldn't really be using this but it's a LOT of work to 
																		   //handle all the cases where we want to replace the old command buffer all
																		   //at once and keep it working, I'll come back to remove direct access to the buffer later
	private:
		VkCommandBuffer m_commandList;
		VkCommandPool m_commandPool;
		VulkanFence* m_fence;
		bool m_workToBeSubmitted;
		CommandBufferType m_type;
		QCore::Delegate<> m_commandBufferCallbackList;
		
	};


}