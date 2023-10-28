#pragma once
#include "render_types.h"

namespace KNR
{
	class VertexDeclaration;

	struct RasterizerState
	{
		CullingMode cullingMode;
		FillMode fillMode;
		float depthBias;
		float depthBiasClamp;
		bool multisampleEnable;
		int sampleCount;
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
		RasterizerState rasterizerState;
		BlendState blendState;
		DepthStencilState depthStencilState;
		VertexDeclaration vertexDeclaration;
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