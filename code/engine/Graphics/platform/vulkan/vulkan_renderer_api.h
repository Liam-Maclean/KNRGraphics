#pragma once

#include "graphics/renderer_api.h"
#include "core/core.h"
#include "vulkan/vulkan.h"
#include "graphics/platform/vulkan/vulkan_util.h";
#include "graphics/platform/vulkan/vulkan_swapchain.h"

namespace Qualia
{
	class VulkanRendererAPI
		: public RendererAPI
	{
	public:
		virtual void Init() override;

		virtual void SetClearColor(const Vector4f& color) override;
		virtual void Clear() const override;

		//virtual void DrawIndexed(const Ref<VertexArray>& vertex_array, uint32_t indexCount = 0, DrawMode mode = Triangles) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		virtual void SetViewport(const float x, const float y, const float width, const float height);
		virtual void BeginRender();
		virtual void EndRender();

	private:

		void SubmitCommandbuffers();
		void CreateSemaphores();


		VulkanSwapchain* m_SwapChain;

		//Clear values for attachments in fragment shader

		size_t currentFrame = 0;
		const int MAX_FRAMES_IN_FLIGHT = 2;
		std::array<VkClearValue, 1> clearValues;
		std::vector<VkCommandBuffer> m_SwapchainCommandBuffers;

		//Semaphores

		std::vector<VkSemaphore> _imageAvailableSemaphores;
		std::vector<VkSemaphore> _renderFinishedSemaphores;
		std::vector<VkFence> _inFlightFences;
	};
}