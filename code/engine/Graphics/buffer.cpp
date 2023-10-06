#include <qlpch.h>
//#include "buffer.h"
//
//#include "platform/opengl/opengl_buffer.h"
//#include "platform/opengl/opengl_vertex_array.h"
//#include "platform/directx/directx_buffer.h"
//#include "platform/directx/directx_vertex_array.h"
//#include "platform/vulkan/vulkan_buffer.h"

namespace Qualia
{
	//Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	//{
	//	switch (Renderer::GetAPI())
	//	{
	//	case RendererAPI::API::None: break;
	//	case RendererAPI::API::OpenGL: return MakeRef<OpenGLVertexBuffer>(size);
	//	case RendererAPI::API::DirectX: return MakeRef<DirectXVertexBuffer>(size);

	//	default:
	//		break;
	//	}
	//	return nullptr;
	//}
	//

	//Ref<IndexBuffer> IndexBuffer::Create(uint32_t size)
	//{
	//	switch (Renderer::GetAPI())
	//	{
	//	case RendererAPI::API::None: break;
	//	case RendererAPI::API::OpenGL: return MakeRef<OpenGLIndexBuffer>(size);
	//	case RendererAPI::API::DirectX: return MakeRef<DirectXIndexBuffer>(size);
	//	//case RendererAPI::API::Vulkan: return nullptr;//MakeRef<VulkanIndexBuffer>(size);

	//	default:
	//		break;
	//	}
	//	return nullptr;
	//}

	//Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	//{
	//	switch (Renderer::GetAPI())
	//	{
	//	case RendererAPI::API::None: break;
	//	case RendererAPI::API::OpenGL: return MakeRef<OpenGLIndexBuffer>(indices, size);
	//	case RendererAPI::API::DirectX: return MakeRef<DirectXIndexBuffer>(indices, size);
	//	//case RendererAPI::API::Vulkan: return MakeRef<VulkanIndexBuffer>(indices, size);

	//	default:
	//		break;
	//	}
	//	return nullptr;
	//}


}