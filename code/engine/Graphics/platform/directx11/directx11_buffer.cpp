#include "directx11_buffer.h"
#include "directx11_util.h"
#include "directx11_graphics_context.h"

namespace KNR
{
	Buffer* Buffer::Create(const BufferDescriptor& desc)
	{
		return new DirectX11Buffer(desc);
	}

	DirectX11Buffer::DirectX11Buffer(const BufferDescriptor& desc)
	{
		switch (desc.bufferAccessType)
		{
		case BufferAccessType::DEFAULT:
			
			break;
		case BufferAccessType::UPLOAD:
		
			break;
		case BufferAccessType::READBACK:
		
			break;
		case BufferAccessType::CUSTOM:
		
			break;
		default:
			break;
		}

		//Create the views for the buffer
		if (desc.bufferType == BufferUsageType::VERTEX)
		{
			
		}
		else if (desc.bufferType == BufferUsageType::INDEX)
		{
			
		}
		else if (desc.bufferType == BufferUsageType::UNIFORM)
		{
		
		}
		else if (desc.bufferType == BufferUsageType::STORAGE)
		{
			
		}

		//If we have initial data, we need to stage
		if (desc.initialData)
		{

		}

		if (desc.debugName)
		{

		}
	}

	DirectX11Buffer::~DirectX11Buffer()
	{
		
	}

	void DirectX11Buffer::Bind(CommandBuffer* commandList) const
	{

	}

	void DirectX11Buffer::Unbind(CommandBuffer* commandList) const
	{

	}

	void* DirectX11Buffer::Map()
	{
		void* data;

		return data;
	}

	void DirectX11Buffer::UnMap()
	{

	}

	uint32_t DirectX11Buffer::AppendData(uint32_t size, void* data)
	{
		uint32_t finalSize;

		return finalSize;
	}
}