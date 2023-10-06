#pragma once
#include "graphics/vertex_array.h"
#include "core/core.h"

namespace Qualia
{
	class VulkanVertexArray
		: public VertexArray
	{
	public:
		VulkanVertexArray();
		virtual ~VulkanVertexArray() {}

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const BufferLayout& GetLayout() const ;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& index_buffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return vertex_buffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return index_buffer; }

	private:
		std::vector<Ref<VertexBuffer>> vertex_buffers;
		Ref<IndexBuffer> index_buffer;
		uint32_t renderer_id;
	};
}