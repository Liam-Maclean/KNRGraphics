#include "buffer.h"
#include "command_buffer.h"
#include "framebuffer.h"
#include "graphics_context.h"
#include "render_commands.h"
#include "shader.h"
#include "technique.h"
#include "texture.h"

void RenderFrame()
{

}

int main()
{
    KNR::Buffer* buffer;
    KNR::Texture* texture;
    KNR::Framebuffer* framebuffer;
    KNR::CommandBuffer* commandBuffer;

    //Initialize KNR (make sure this is done before initializing any KNR types)
    KNR::RenderCommand::Initialize();

    //Create a framebuffer (can be a swap chain backbuffer)
    KNR::FramebufferTextureSpecification textureSpec = {};
    textureSpec.textureFormat = KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_UNORM;
    textureSpec.samples = 1;

    KNR::FramebufferAttachmentSpecification attachmentSpec = {};
    attachmentSpec.Attachments.push_back(textureSpec);

    KNR::FramebufferSpecification framebufferSpec = {};
    framebufferSpec.Width = 1920;
    framebufferSpec.Height = 1080;
    framebufferSpec.Samples = 1;
    framebufferSpec.SwapChainTarget = true;
    framebufferSpec.Attachments = attachmentSpec;
    framebuffer = KNR::Framebuffer::Create(framebufferSpec);

    //Create a buffer using a descriptor
    KNR::BufferDescriptor bufferDesc = {};
    bufferDesc.bufferType = KNR::BufferUsageType::VERTEX;
    bufferDesc.bufferAccessType = KNR::BufferAccessType::DEFAULT;
    bufferDesc.vertexBuffer.stride = 0;
    buffer = KNR::Buffer::Create(bufferDesc);

    //Create a command buffer
    KNR::CommandBufferType commandBufferType = KNR::CommandBufferType::graphics;
    commandBuffer = KNR::CommandBuffer::Create(commandBufferType);

    KNR::RenderCommand::BeginRender();
    KNR::RenderCommand::SetViewport(0, 0, 1920, 1080);
    KNR::RenderCommand::Clear();

    framebuffer->Bind(commandBuffer);

    framebuffer->Unbind(commandBuffer);

    KNR::RenderCommand::SetSwapchainRenderTarget();
    KNR::RenderCommand::EndRender(); //End rendering
    KNR::RenderCommand::Present(); //Present to swapchain

    return 0;
}