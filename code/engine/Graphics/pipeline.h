#pragma once
#include "render_types.h"
#include "vertex_declaration.h"
namespace KNR
{
	struct ShaderBytecode
	{
		void* bytecode;
		size_t size;
	};

	struct RasterizerState
	{
		CullingMode cullingMode;
		FillMode fillMode;
		float depthBias;
		float depthBiasClamp;
		bool multisampleEnable;
		int sampleCount;
		bool conservativeRasterizer;
	};

	struct DepthStencilState
	{
		bool depthEnable;
		DepthComparisonOp depthComparisonOp;
		bool stencilEnable;
		UINT8 stencilReadMask;
		UINT8 stencilWriteMask;
		//StencilOp stencilFrontFace; TODO - Stencil ops front face
		//StencilOp stencilBackFace; TODO - Stencil ops back face
	};

	struct RenderTargetBlendState
	{
		bool blendEnable;
		bool logicOpEnable;
		BlendValue blendSrc;
		BlendValue blendDest;
		BlendOp blendOp;
		BlendValue blendSrcAlpha;
		BlendValue blendDestAlpha;
		BlendOp blendAlphaOp;
		UINT8 writeMask;
	};

	struct BlendState
	{
		bool alphaToCoverageEnable;
		bool independentBlendEnable;
		RenderTargetBlendState renderTargetBlendStates[8];
	};

	struct PipelineStateDesc
	{
		ShaderBytecode vertexBytecode;
		ShaderBytecode pixelBytecode;
		ShaderBytecode computeBytecode;
		ShaderBytecode hullBytecode;
		ShaderBytecode domainBytecode;
		RasterizerState rasterizerState;
		BlendState blendState;
		DepthStencilState depthStencilState;
		VertexDeclaration vertexDeclaration;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual void Bind(CommandBuffer* commandList) = 0;
		virtual void Unbind(CommandBuffer* commandList) = 0;

		static Pipeline* Create(const PipelineStateDesc& desc);
	protected:
		std::string m_name;
	};
}