#include "renderer_api.h"

namespace KNR
{
#if defined(PLATFORM_OPENGL)
	RendererAPI::API RendererAPI::s_target_api = RendererAPI::API::OpenGL;
#elif defined(PLATFORM_VULKAN)
	RendererAPI::API RendererAPI::s_target_api = RendererAPI::API::Vulkan;
#elif defined(PLATFORM_DX11)
	RendererAPI::API RendererAPI::s_target_api = RendererAPI::API::DirectX11;
#elif defined(PLATFORM_DX12)
	RendererAPI::API RendererAPI::s_target_api = RendererAPI::API::DirectX12;
#endif
}