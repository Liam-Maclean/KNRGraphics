#include "render_commands.h"

#ifdef PLATFORM_OPENGL
#include "platform/opengl/opengl_renderer_api.h"
#elif PLATFORM_VULKAN
#include "platform/vulkan/vulkan_renderer_api.h";
#elif PLATFORM_DX12
#include "platform/directx/directx_renderer_api.h"
#elif PLATFORM_DX11
#endif // OPEN_GL


namespace KNR
{
#ifdef PLATFORM_VULKAN
	RendererAPI* RenderCommand::sRendererAPI = new DirectXRendererAPI;
#elif PLATFORM_VULKAN
	RendererAPI* RenderCommand::sRendererAPI = new VulkanRendererAPI;
#elif PLATFORM_DX11
	RendererAPI* RenderCommand::sRendererAPI = new DirectXRendererAPI;
#elif PLATFORM_DX12
	RendererAPI* RenderCommand::sRendererAPI = new DirectXRendererAPI;
#elif PLATFORM_DX12
	RendererAPI* RenderCommand::sRendererAPI = new DirectXRendererAPI;
#endif
}