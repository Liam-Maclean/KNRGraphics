#pragma once
#include "render_types.h"
#include "vertex_declaration.h"
namespace KNR
{
	struct PipelineStateDesc
	{
		ShaderBytecode vertexBytecode;
		ShaderBytecode pixelBytecode;
		ShaderBytecode computeBytecode;
		ShaderBytecode hullBytecode;
		ShaderBytecode domainBytecode;
		RasterizerState rasterizerState;
		RenderTargetState renderTargetState;
		BlendState blendState;
		DepthStencilState depthStencilState;
		VertexDeclaration* vertexDeclaration;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		static Pipeline* Create(const PipelineStateDesc& desc);
	protected:
		std::string m_name;
	};
}