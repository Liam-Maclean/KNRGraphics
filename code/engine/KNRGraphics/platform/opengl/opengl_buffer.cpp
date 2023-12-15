#include "opengl_buffer.h"
#include "opengl_util.h"
#include "opengl_graphics_context.h"

namespace KNR
{
	Buffer* Buffer::Create(const BufferDescriptor& desc)
	{
		return new OpenglBuffer(desc);
	}

	OpenglBuffer::OpenglBuffer(const BufferDescriptor& desc)
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

	OpenglBuffer::~OpenglBuffer()
	{
		
	}

	void OpenglBuffer::Bind(CommandBuffer* commandList) const
	{

	}

	void OpenglBuffer::Unbind(CommandBuffer* commandList) const
	{

	}

	void* OpenglBuffer::Map()
	{
		void* data;

		return data;
	}

	void OpenglBuffer::UnMap()
	{

	}

	uint32_t OpenglBuffer::AppendData(uint32_t size, void* data)
	{
		uint32_t finalSize;

		return finalSize;
	}
}