#include <qlpch.h>
#include "render_commands.h"

#ifdef OPENGL
#include "platform/opengl/opengl_renderer_api.h"
#elif VULKAN
#include "platform/vulkan/vulkan_renderer_api.h";
#elif DIRECTX
#include "platform/directx/directx_renderer_api.h"
#endif // OPEN_GL


namespace QRender
{
#ifdef VULKAN
	RendererAPI* RenderCommand::sRendererAPI = new VulkanRendererAPI;
#elif DIRECTX
	RendererAPI* RenderCommand::sRendererAPI = new DirectXRendererAPI;
#endif
}