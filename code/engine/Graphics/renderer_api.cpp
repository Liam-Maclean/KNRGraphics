#include "renderer_api.h"

namespace QRender
{
#if defined(OPENGL)
	RendererAPI::API RendererAPI::s_target_api = RendererAPI::API::OpenGL;
#elif defined(VULKAN)
	RendererAPI::API RendererAPI::s_target_api = RendererAPI::API::Vulkan;
#elif defined(DIRECTX)
	RendererAPI::API RendererAPI::s_target_api = RendererAPI::API::DirectX;
	//#error "Vulkan not yet implemented."
#endif
}