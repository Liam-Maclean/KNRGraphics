#pragma 
#include "window.h"

//Forward decl's
namespace KNR
{
    class Texture2D;
    class Buffer;
    class CommandBuffer;
    class Shader;
    class Pipeline;
    class VertexDeclaration;
}

class GraphicsApplication
{
public:
	GraphicsApplication();

	~GraphicsApplication();

	void Initialise(const KNR::WindowDesc& windowDesc);

	void Update();

	void Render();

private:

    KNR::Texture2D* m_renderTargetColor;
    KNR::Texture2D* m_renderTargetNormal;
    KNR::Texture2D* m_renderTargetDepth;
    KNR::Buffer* m_constantIndexBuffer;
    KNR::Buffer* m_materialIndexBuffer;
    KNR::Buffer* m_cameraDataBuffer;
    KNR::Buffer* m_frameDataBuffer;
    KNR::CommandBuffer* m_commandBuffer;
    KNR::VertexDeclaration* m_vertexDeclaration;
    KNR::Pipeline* m_pipelineState;
    KNR::Shader* m_vertexShader;
    KNR::Shader* m_pixelShader;
};