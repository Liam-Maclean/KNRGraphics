#pragma once
#include <cassert>
#include <string>
#include <vector>
#include <Windows.h>
namespace KNR
{
	class VertexDeclaration;
	class CommandBuffer;
	enum class BufferUsageType
	{
		Vertex,
		Index,
		Storage,
		Uniform,
		Copy,
		Upload
	};

	enum class BufferAccessType
	{
		Default,	//GPU Visible only
		Upload,		//CPU & GPU visible
		Readback,	//Readback (Queries)
		Custom		//Custom (UMA/Multi-graphics card optimisations)
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
		VertexDeclaration* vertexDeclaration;
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

		virtual void* Map() = 0;
		virtual void UnMap() = 0;

		virtual size_t GetCapacitySize() const = 0;
		virtual size_t GetUsedSize() const = 0;

		static Buffer* Create(const BufferDescriptor& desc);
	};
}