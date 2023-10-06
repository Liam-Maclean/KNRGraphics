#include "qlpch.h"
#include "opengl_vertex_array.h"

#include <glad/glad.h>

namespace Qualia
{
	Ref<VertexArray> VertexArray::Create()
	{
		return MakeRef<OpenGLVertexArray>();
	}

	GLenum ShaderDataTypeToOpenGLBase(ShaderDataType type)
	{
		switch (type)
		{
		case  ShaderDataType::Float:
		case  ShaderDataType::Float2:
		case  ShaderDataType::Float3:
		case  ShaderDataType::Float4:
		case  ShaderDataType::Mat3:
		case  ShaderDataType::Mat4:
			return GL_FLOAT;

		case  ShaderDataType::Int:
		case  ShaderDataType::Int2:
		case  ShaderDataType::Int3:
		case  ShaderDataType::Int4:
			return GL_INT;

		case  ShaderDataType::Bool:
			return GL_BOOL;
		}
		assert(false);
		return 0;
	}


	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_rendererId);
	}

	void  OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_rendererId);
	}

	void  OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void  OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		glBindVertexArray(m_rendererId);
		vertexBuffer->Bind();

		uint32_t index = 0;

		const auto& layout = vertexBuffer->GetLayout();

		for (const auto& element : layout.GetElements())
		{
			switch (element.type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(m_vertexBufferIndex);
				glVertexAttribPointer(m_vertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBase(element.type),
					element.normalised ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)element.offset);
				m_vertexBufferIndex++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(m_vertexBufferIndex);
				glVertexAttribIPointer(m_vertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBase(element.type),
					layout.GetStride(),
					(const void*)element.offset);
				m_vertexBufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(m_vertexBufferIndex);
					glVertexAttribPointer(m_vertexBufferIndex,
						count,
						ShaderDataTypeToOpenGLBase(element.type),
						element.normalised ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.offset + sizeof(float) * count * i));
					glVertexAttribDivisor(m_vertexBufferIndex, 1);
					m_vertexBufferIndex++;
				}
				break;
			}
			default:
				// assert 
				break;
			}
		}
		m_vertexBuffers.emplace_back(vertexBuffer);
	}

	void  OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_rendererId);
		indexBuffer->Bind();
		this->m_indexBuffer = indexBuffer;
	}

}