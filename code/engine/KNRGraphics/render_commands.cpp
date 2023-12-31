#include "render_commands.h"

#ifdef PLATFORM_OPENGL
#include "platform/opengl/opengl_renderer_api.h"
#elif PLATFORM_VULKAN
#include <platform/vulkan/vulkan_renderer_api.h>;
#elif PLATFORM_DX12
#include "platform/directx12/directx12_renderer_api.h"
#elif PLATFORM_DX11
#include "platform/directx11/directx11_renderer_api.h"
#endif // OPEN_GL


namespace KNR
{
#ifdef PLATFORM_OPENGL
	RendererAPI* RenderCommand::sRendererAPI = new OpenglRendererAPI;
#elif PLATFORM_VULKAN
	RendererAPI* RenderCommand::sRendererAPI = new VulkanRendererAPI;
#elif PLATFORM_DX11
	RendererAPI* RenderCommand::sRendererAPI = new DirectX11RendererAPI;
#elif PLATFORM_DX12
	RendererAPI* RenderCommand::sRendererAPI = new DirectX12RendererAPI;
#endif
}