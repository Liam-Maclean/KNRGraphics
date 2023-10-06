
#include "opengl_buffer.h"
#include "glad/glad.h"

namespace Qualia
{
	///////////////////////////////////////////////////////////////////////
	// Vertex Buffer
	///////////////////////////////////////////////////////////////////////

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		return MakeRef<OpenGLVertexBuffer>(size);
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		return MakeRef<OpenGLVertexBuffer>(vertices, size);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		glCreateBuffers(1, &m_rendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float * vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_rendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_rendererId);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		
	}

	///////////////////////////////////////////////////////////////////////
	// Index Buffer
	///////////////////////////////////////////////////////////////////////

	Ref<IndexBuffer> Create(uint32_t size)
	{
		return MakeRef<OpenGLIndexBuffer>(size);
	}

	Ref<IndexBuffer> Create(uint32_t* indices, uint32_t size)
	{
		return MakeRef<OpenGLIndexBuffer>(indices, size);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
		: m_count(count)
	{
		glCreateBuffers(1, &m_rendererId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_count(count)
	{
		glCreateBuffers(1, &m_rendererId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_rendererId);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);
	}
	
	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	uint32_t OpenGLIndexBuffer::GetCount() const
	{
		return m_count;
	}
	
	void OpenGLIndexBuffer::SetData(const void* data, uint32_t count)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(uint32_t), data);
	}
}
