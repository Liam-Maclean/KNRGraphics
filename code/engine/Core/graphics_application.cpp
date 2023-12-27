#include "graphics_application.h"

//KNR Graphics API
#include "KNR_ShaderInterop.h"
#include "buffer.h"
#include "command_buffer.h"
#include "graphics_context.h"
#include "render_commands.h"
#include "shader.h"
#include "texture.h"
#include "pipeline.h"

GraphicsApplication::GraphicsApplication()
{

}

GraphicsApplication::~GraphicsApplication()
{

}

void GraphicsApplication::Initialise(const KNR::WindowDesc& windowDesc)
{
	KNR::RenderCommand::Initialize(windowDesc);

    //Create a command buffer
    KNR::CommandBufferType commandBufferType = KNR::CommandBufferType::Graphics;
    m_commandBuffer = KNR::CommandBuffer::Create(commandBufferType);

    KNR::TextureDescriptor renderTargetColorDesc = {};
    renderTargetColorDesc.textureType = KNR::TextureType::Framebuffer;
    renderTargetColorDesc.textureUsage = KNR::TextureUsage::RenderTarget;
    renderTargetColorDesc.textureFormat = KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_FLOAT;
    renderTargetColorDesc.width = 1920;
    renderTargetColorDesc.height = 1080;
    renderTargetColorDesc.mipCount = 1;
    renderTargetColorDesc.samples = 1;

    KNR::TextureDescriptor renderTargetDepthDesc = {};
    renderTargetDepthDesc.textureType = KNR::TextureType::Framebuffer;
    renderTargetDepthDesc.textureUsage = KNR::TextureUsage::RenderTarget;
    renderTargetDepthDesc.textureFormat = KNR::TextureFormat::TEXTURE_FORMAT_D24_UNORM_S8_UINT;
    renderTargetDepthDesc.width = 1920;
    renderTargetDepthDesc.height = 1080;
    renderTargetDepthDesc.mipCount = 1;
    renderTargetDepthDesc.samples = 1;

    m_renderTargetColor = KNR::Texture2D::Create(renderTargetColorDesc);
    m_renderTargetDepth = KNR::Texture2D::Create(renderTargetDepthDesc);

    KNR::CameraData cameraData;
    cameraData.MVPMatrix = XMMatrixIdentity();
    cameraData.InverseProjectionMatrix = XMMatrixIdentity();
    cameraData.InverseViewMatrix = XMMatrixIdentity();
    cameraData.ModelMatrix = XMMatrixIdentity();
    XMStoreFloat4(&cameraData.EyePos, (XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)));
    cameraData.z_near = 0.1f;
    cameraData.z_far = 10000.0f;

    KNR::FrameData frameData;

    KNR::BufferDescriptor frameDataBufferDesc = {};
    frameDataBufferDesc.bufferAccessType = KNR::BufferAccessType::Upload;
    frameDataBufferDesc.bufferType = KNR::BufferUsageType::Bindless;
    frameDataBufferDesc.size = sizeof(KNR::FrameData);
    frameDataBufferDesc.initialData = (void*)&frameData;
    frameDataBufferDesc.debugName = L"Frame data buffer";
    m_frameDataBuffer = KNR::Buffer::Create(frameDataBufferDesc);

    KNR::BufferDescriptor cameraDataBufferDesc = {};
    cameraDataBufferDesc.bufferAccessType = KNR::BufferAccessType::Upload;
    cameraDataBufferDesc.bufferType = KNR::BufferUsageType::Bindless;
    cameraDataBufferDesc.size = sizeof(KNR::CameraData);
    cameraDataBufferDesc.initialData = (void*)&cameraData;
    cameraDataBufferDesc.debugName = L"Camera data buffer";
    m_cameraDataBuffer = KNR::Buffer::Create(cameraDataBufferDesc);

    //Indices into the bindless buffers (Shader side)
    KNR::ConstantIndexingData constantIndexingData = {};
    constantIndexingData.constantIndexCameraDataBuffer = m_cameraDataBuffer->GetBindlessIndex();
    constantIndexingData.constantIndexFrameDataBuffer = m_frameDataBuffer->GetBindlessIndex();

    KNR::BufferDescriptor constantIndexingBufferDesc = {};
    constantIndexingBufferDesc.bufferAccessType = KNR::BufferAccessType::Upload;
    constantIndexingBufferDesc.bufferType = KNR::BufferUsageType::Uniform;
    constantIndexingBufferDesc.size = sizeof(KNR::ConstantIndexingData);
    constantIndexingBufferDesc.initialData = (void*)&constantIndexingData;
    constantIndexingBufferDesc.debugName = L"Constant indexing buffer";
    m_constantIndexBuffer = KNR::Buffer::Create(constantIndexingBufferDesc);

    KNR::VertexAttribute attributes[2] =
    {
        KNR::VertexAttribute(KNR::VertexAttributeFormat::Float, KNR::VertexAttributeUsage::Position, 4, 16, 0),
        KNR::VertexAttribute(KNR::VertexAttributeFormat::Float, KNR::VertexAttributeUsage::Texcoord, 2, 8, 0),
    };
    m_vertexDeclaration = KNR::VertexDeclaration::Create(attributes, 2);

    m_vertexShader = KNR::Shader::Create("shaders/forward_draw_vert.dxc");
    m_pixelShader = KNR::Shader::Create("shaders/forward_draw_pixel.dxc");

    KNR::PipelineStateDesc pipelineStateDesc = {};
    pipelineStateDesc.vertexBytecode = m_vertexShader->GetShaderBytecode();
    pipelineStateDesc.pixelBytecode = m_pixelShader->GetShaderBytecode();
    pipelineStateDesc.vertexDeclaration = m_vertexDeclaration;
    pipelineStateDesc.rasterizerState = KNR::RasterizerState();
    pipelineStateDesc.blendState = KNR::BlendState();
    pipelineStateDesc.depthStencilState = KNR::DepthStencilState();
    pipelineStateDesc.renderTargetState.m_ColorTargets.push_back(m_renderTargetColor->GetFormat());
    pipelineStateDesc.renderTargetState.m_DepthTarget = m_renderTargetDepth->GetFormat();

    m_pipelineState = KNR::Pipeline::Create(pipelineStateDesc);
}

void GraphicsApplication::Update()
{
}

void GraphicsApplication::Render()
{
    //Begin rendering to backbuffer
    KNR::RenderCommand::BeginFrame();

    //Start recording commands in a command buffer
    KNR::RenderCommand::BeginRecordingCommands(m_commandBuffer);

    KNR::RenderCommand::SetScissor(m_commandBuffer, 0, 0, m_renderTargetColor->GetWidth(), m_renderTargetColor->GetHeight());
    KNR::RenderCommand::SetViewport(m_commandBuffer, 0, 0, m_renderTargetColor->GetWidth(), m_renderTargetColor->GetHeight());

    //Bind render targets we're rendering to
    KNR::Texture2D* renderTargets[1] =
    { 
        m_renderTargetColor,
    };
    KNR::RenderCommand::BindRenderTargets(m_commandBuffer, m_renderTargetColor, 1, m_renderTargetDepth);

    //Bind the fixed function pipeline (Rasterizer, input assembler, shaders, depth stencil, blend modes)
    KNR::RenderCommand::BindPipeline(m_commandBuffer, m_pipelineState);

    //Bind the indices used to fetch uniforms;
    KNR::RenderCommand::BindUniformBuffer(m_commandBuffer, m_constantIndexBuffer, (int)KNR::KNRRootSignatureBindSlot::ConstantIndexSlot);

    //Draw a triangle
    KNR::RenderCommand::ImmediateDrawBegin(KNR::TopologyIndexMethod::Trianglelist);
    KNR::RenderCommand::ImmediateVertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    KNR::RenderCommand::ImmediateVertex(1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    KNR::RenderCommand::ImmediateVertex(1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    KNR::RenderCommand::ImmediateDrawEnd(m_commandBuffer);

    //Submit commands
    KNR::RenderCommand::SubmitCommandBufferImmediate(m_commandBuffer);

    KNR::RenderCommand::EndFrame(); //End rendering
    KNR::RenderCommand::Present(); //Present to swapchain
}
