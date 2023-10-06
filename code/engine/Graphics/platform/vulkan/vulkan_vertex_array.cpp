#include "qlpch.h"
#include "vulkan_vertex_array.h"

#include <glad/glad.h>

namespace Qualia
{
	VulkanVertexArray::VulkanVertexArray()
	{
		//glCreateVertexArrays(1, &renderer_id);
	}

	void  VulkanVertexArray::Bind() const
	{
		//glBindVertexArray(renderer_id);
	}

	void  VulkanVertexArray::Unbind() const
	{
		//glBindVertexArray(0);
	}

	const BufferLayout& VulkanVertexArray::GetLayout() const
	{
		return BufferLayout();
	}

	void  VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer)
	{
		//glBindVertexArray(renderer_id);
		//vertex_buffer->Bind();
		//
		//uint32_t index = 0;
		//
		//const auto& layout = vertex_buffer->GetLayout();
		//
		//for (const auto& element : layout.GetElements())
		//{
		//	glVertexAttribPointer(index,
		//		element.GetComponentCount(),
		//		ShaderDataTypeToOpenGLBase(element.type),
		//		element.normalised ? GL_TRUE : GL_FALSE,
		//		layout.GetStride(),
		//		(const void*)element.offset);
		//	glEnableVertexAttribArray(index);
		//	index++;
		//}
		vertex_buffers.emplace_back(vertex_buffer);
	}

	void  VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& index_buffer)
	{
		//glBindVertexArray(renderer_id);
		//index_buffer->Bind();
		this->index_buffer = index_buffer;
	}

}