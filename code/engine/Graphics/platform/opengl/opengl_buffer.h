#pragma once
#include <cstdint>

#include "graphics/vertex_array.h"

namespace Qualia
{
	class OpenGLVertexBuffer
		: public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

		virtual const BufferLayout& GetLayout() const override { return m_layout; }
		virtual void SetLayout(const BufferLayout& layout) override { this->m_layout = layout; }

	private:
		uint32_t m_rendererId;
		BufferLayout m_layout;
	};

	class OpenGLIndexBuffer 
		: public Qualia::IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t size);
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		~OpenGLIndexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		virtual uint32_t GetCount() const override;
		void SetData(const void* data, uint32_t count) override;

	private:
		uint32_t m_rendererId;
		uint32_t m_count;
	};
}