#pragma once
#include "core/core.h"
#include "vertex.h"
#include <cassert>
#include <string>
#include <vector>

namespace QRender
{
	class DirectXCommandBuffer;


	///////////////////////////////////////////////////////////////////////
	// Shader Data Types
	///////////////////////////////////////////////////////////////////////

	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return 4 ;
			case ShaderDataType::Float2:	return 4 * 2 ;
			case ShaderDataType::Float3:	return 4 * 3 ;
			case ShaderDataType::Float4:	return 4 * 4 ;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2  ;
			case ShaderDataType::Int3:		return 4 * 3 ;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;
			case ShaderDataType::None:		return 0;
		}
	
		Q_CORE_DEBUG("Unknown shader data type!");
		return 0;
	}

	///////////////////////////////////////////////////////////////////////
	// Buffer Element
	///////////////////////////////////////////////////////////////////////

	struct BufferElement
	{
		std::string name = {};
		ShaderDataType type = {};
		uint32_t offset = {};
		uint32_t size = {};
		bool normalised = {};

		BufferElement() {}

		BufferElement(ShaderDataType type, const std::string& name, bool normalised = false)
			: name(name)
			, type(type)
			, offset(0)
			, size(ShaderDataTypeSize(type))
			, normalised(normalised)
		{

		}

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
				case ShaderDataType::Float:		return 1;
				case ShaderDataType::Float2:	return 2;
				case ShaderDataType::Float3:	return 3;
				case ShaderDataType::Float4:	return 4;
				case ShaderDataType::Mat3:		return 3*3*3;
				case ShaderDataType::Mat4:		return 4*4*4;
				case ShaderDataType::Int:		return 1;
				case ShaderDataType::Int2:		return 2;
				case ShaderDataType::Int3:		return 3;
				case ShaderDataType::Int4:		return 4;
				case ShaderDataType::Bool:		return 1;
			}

			assert(false);
			return 0;
		}
	};

	///////////////////////////////////////////////////////////////////////
	// Buffer Layout
	///////////////////////////////////////////////////////////////////////
	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements) 
			:  elements(elements)
		{
			CalculateOffsetAndStride();
		}

		inline const std::vector<BufferElement>& GetElements() const
		{
			return elements;
		}

		inline uint32_t GetStride() const 
		{
			return stride;
		}

		std::vector<BufferElement>::iterator begin() { return elements.begin(); }
		std::vector<BufferElement>::iterator end() { return elements.end(); }

	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			stride = 0;
			for (auto& element : elements)
			{
				element.offset = offset;
				offset += element.size;
				stride += element.size;
			}
		}

	private:
		std::vector<BufferElement> elements;
		uint32_t stride = 0;
	};


	enum class BufferUsageType
	{
		VERTEX,
		INDEX,
		STORAGE,
		UNIFORM,
		COPY,
		UPLOAD
	};

	enum class BufferAccessType
	{
		DEFAULT,	//GPU Visible only
		UPLOAD,		//CPU & GPU visible
		READBACK,	//Readback (Queries)
		CUSTOM		//Custom (UMA/Multi-graphics card optimisations)
	};

	struct IndexBufferDesc
	{
		uint32_t format;
	};

	struct ReadbackBufferDesc
	{
		uint32_t bufferWidth;
	};
	struct VertexBufferDesc
	{
		uint32_t stride;
	};

	struct StructuredBufferDesc
	{
		uint32_t numElements;
		uint32_t structuredByteStride;
	};

	struct BufferDescriptor
	{
		uint32_t size;
		void* initialData;
		BufferUsageType bufferType;
		BufferAccessType bufferAccessType;
		union
		{
			IndexBufferDesc indexBuffer;
			VertexBufferDesc vertexBuffer;
			ReadbackBufferDesc readbackBuffer;
			StructuredBufferDesc structuredBuffer;
		};
		LPCWSTR debugName;
	};

	class Buffer
	{
	public:
		Buffer() {}
		virtual ~Buffer() {}

		virtual void Bind(Ref<DirectXCommandBuffer> commandList) const = 0;
		virtual void Unbind(Ref<DirectXCommandBuffer> commandList) const = 0;

		virtual void* Map() = 0;
		virtual void UnMap() = 0;

		virtual uint32_t AppendData(uint32_t size, void* data) = 0;

		virtual size_t GetCapacitySize() = 0;
		virtual size_t GetUsedSize() = 0;

		static Ref<Buffer> Create(const BufferDescriptor& desc);
	};
}