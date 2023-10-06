#include "vulkan_renderer_api.h"
#include "graphics/vertex_array.h"

void Qualia::VulkanRendererAPI::Init()
{
	CreateSemaphores();
	m_SwapChain = new VulkanSwapchain();
}

void Qualia::VulkanRendererAPI::SetClearColor(const Vector4f& color)
{	
	clearValues[0].color = { color.x, color.y, color.z, color.a};
}

void Qualia::VulkanRendererAPI::Clear() const
{

}

void Qualia::VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
{
	for (int i = 0; i < m_SwapchainCommandBuffers.size(); ++i)
	{
		vkCmdDrawIndexed(m_SwapchainCommandBuffers[i], vertexArray->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}
}

void Qualia::VulkanRendererAPI::SetViewport(const float x, const float y, const float width, const float height)
{

	for (int i = 0; i < m_SwapchainCommandBuffers.size(); ++i)
	{
		VkViewport viewport = {};
		viewport.x = x;
		viewport.y = y;
		viewport.width = (float)width;
		viewport.height = (float)height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_SwapchainCommandBuffers[i], 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = m_SwapChain->GetSwapchainExtent().width;
		scissor.extent.height = m_SwapChain->GetSwapchainExtent().height;
		scissor.offset = { 0,0 };
		vkCmdSetScissor(m_SwapchainCommandBuffers[i], 0, 1, &scissor);

	}
}

void Qualia::VulkanRendererAPI::BeginRender()
{
	if (m_SwapchainCommandBuffers.size() != m_SwapChain->GetSwapchainFramebuffers().size())
	{
		m_SwapchainCommandBuffers.resize(m_SwapChain->GetSwapchainFramebuffers().size());
		m_SwapchainCommandBuffers[0] = VK_NULL_HANDLE;
		m_SwapchainCommandBuffers[1] = VK_NULL_HANDLE;
		m_SwapchainCommandBuffers[2] = VK_NULL_HANDLE;
	}


	if (m_SwapchainCommandBuffers[0] == VK_NULL_HANDLE)
	{
		VkCommandBufferAllocateInfo command_buffer_allocate_info{};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.commandPool = Vulkan::Utility::GetInstance().GetCommandPool();
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = static_cast<uint32_t>(m_SwapchainCommandBuffers.size());

		Qualia::Vulkan::Utility::VulkanErrorCheck(vkAllocateCommandBuffers(VulkanDevice::GetInstance().GetVulkanDevice(), &command_buffer_allocate_info, m_SwapchainCommandBuffers.data()));
	}

	//set up cmd buffer begin info
	VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	//begin to set up the information for the render pass
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	renderPassBeginInfo.renderPass = m_SwapChain->GetSwapchainRenderPass();					// we still need to do this (graphics pipeline work)
	renderPassBeginInfo.renderArea.extent.width = m_SwapChain->GetSwapchainExtent().width;
	renderPassBeginInfo.renderArea.extent.height = m_SwapChain->GetSwapchainExtent().height;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();

	for (int i = 0; i < m_SwapchainCommandBuffers.size(); ++i)
	{
		renderPassBeginInfo.framebuffer = m_SwapChain->GetSwapchainFramebuffers()[i];

		//begin command buffer and start the render pass
		Qualia::Vulkan::Utility::VulkanErrorCheck(vkBeginCommandBuffer(m_SwapchainCommandBuffers[i], &cmdBufferBeginInfo));
		vkCmdBeginRenderPass(m_SwapchainCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		//Bind the pipeline for rendering to the swapchain
		vkCmdBindPipeline(m_SwapchainCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_SwapChain->GetSwapchainGraphicsPipeline());


		//In bind Vertex and Bind Index buffer functions somewhere else in code
		//vkCmdBindVertexBuffers(_drawCommandBuffers[i], 0, 1, &screenTarget->GetVertexBuffer()->buffer, offsets);
		//vkCmdBindIndexBuffer(_drawCommandBuffers[i], screenTarget->GetIndexBuffer()->buffer, 0, VK_INDEX_TYPE_UINT32);
	}


	
}

void Qualia::VulkanRendererAPI::EndRender()
{
	for (int i = 0; i < m_SwapchainCommandBuffers.size(); ++i)
	{
		vkCmdEndRenderPass(m_SwapchainCommandBuffers[i]);

	}


	SubmitCommandbuffers();
}

void Qualia::VulkanRendererAPI::SubmitCommandbuffers()
{
	
	vkWaitForFences(VulkanDevice::GetInstance().GetVulkanDevice(), 1, &_inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(VulkanDevice::GetInstance().GetVulkanDevice(), m_SwapChain->GetSwapchain(), std::numeric_limits<uint64_t>::max(),
		_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//_RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	//_UpdateUniformBuffer(imageIndex);

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = waitSemaphores;
	submit_info.pWaitDstStageMask = waitStages;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_SwapchainCommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[currentFrame] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signalSemaphores;
	//
	vkResetFences(VulkanDevice::GetInstance().GetVulkanDevice(), 1, &_inFlightFences[currentFrame]);
	vkQueueSubmit(VulkanDevice::GetInstance().GetVulkanGraphicsQueue(), 1, &submit_info, _inFlightFences[currentFrame]);

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_SwapChain->GetSwapchain() };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapChains;
	present_info.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(VulkanDevice::GetInstance().GetVulkanPresentQueue(), &present_info);
	//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _frameBufferResized)
	//{
	//	_frameBufferResized = false;
	//	//_RecreateSwapChain();
	//}
	//else if (result != VK_SUCCESS)
	//{
	//	throw std::runtime_error("Failed to present swap chain image");
	//}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Qualia::VulkanRendererAPI::CreateSemaphores()
{
	_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		Qualia::Vulkan::Utility::VulkanErrorCheck(vkCreateSemaphore(VulkanDevice::GetInstance().GetVulkanDevice(), &semaphore_create_info, nullptr, &_imageAvailableSemaphores[i]));
		Qualia::Vulkan::Utility::VulkanErrorCheck(vkCreateSemaphore(VulkanDevice::GetInstance().GetVulkanDevice(), &semaphore_create_info, nullptr, &_renderFinishedSemaphores[i]));
		Qualia::Vulkan::Utility::VulkanErrorCheck(vkCreateFence(VulkanDevice::GetInstance().GetVulkanDevice(), &fence_create_info, nullptr, &_inFlightFences[i]));
	}
}
