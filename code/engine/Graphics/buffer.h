#pragma once
#include "vertex.h"
#include <cassert>
#include <string>
#include <vector>
#include <Windows.h>
namespace KNR
{
	class CommandBuffer;
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

		virtual void Bind(CommandBuffer* commandList) const = 0;
		virtual void Unbind(CommandBuffer* commandList) const = 0;

		virtual void* Map() = 0;
		virtual void UnMap() = 0;

		virtual uint32_t AppendData(const uint32_t size, const void* data) = 0;

		virtual size_t GetCapacitySize() const = 0;
		virtual size_t GetUsedSize() const = 0;

		static Buffer* Create(const BufferDescriptor& desc);
	};
}