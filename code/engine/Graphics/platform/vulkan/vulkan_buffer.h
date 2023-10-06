#pragma once
#include <cstdint>
#include "graphics/vertex_array.h"
#include "vulkan/vulkan.h"

namespace Qualia
{
	class VulkanVertexBuffer
		: public VertexBuffer
	{
	public:
		VulkanVertexBuffer(float* vertices, uint32_t size);
		~VulkanVertexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		virtual const BufferLayout& GetLayout() const override { return layout; }
		virtual void SetLayout(const BufferLayout& layout) override { this->layout = layout; }
		virtual void SetData(const void* data, uint32_t size, uint32_t offset) {};
		
	private:
		uint32_t renderer_id;
		BufferLayout layout;


		VkDevice device;
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDescriptorBufferInfo descriptor;
		void* mapped = nullptr;

	};

	class VulkanIndexBuffer
		: public Qualia::IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* indices, uint32_t size);
		~VulkanIndexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override{}
		virtual uint32_t GetCount() const override;

	private:
		uint32_t renderer_id;
		uint32_t count;

		VkDevice device;
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDescriptorBufferInfo descriptor;
		void* mapped = nullptr;
	};

	//class VulkanUniformBuffer
	//	: public Qualia::UniformBuffer
	//{
	//public:
	//	VulkanUniformBuffer(void* data, uint32_t size);
	//	~VulkanUniformBuffer() override;
	//
	//	void Bind() const override;
	//	void Unbind() const override;
	//
	//	virtual const BufferLayout& GetLayout() const override { return layout; }
	//	virtual void SetLayout(const BufferLayout& layout) override { this->layout = layout; }
	//
	//private:
	//	uint32_t renderer_id;
	//	BufferLayout layout;
	//
	//
	//	VkDevice device;
	//	VkBuffer buffer;
	//	VkDeviceMemory memory;
	//	VkDescriptorBufferInfo descriptor;
	//	void* mapped = nullptr;
	//
	//};
}