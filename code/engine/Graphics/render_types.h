#pragma once

namespace KNR
{
	struct GPUInfoCache
	{
		char hardwareNames[128];
		uint32_t vramSize;
		uint32_t availableMemory;
		bool rtxSupport;
	};

	struct HardwareGPUTelemetry
	{
		uint32_t gpuCount;
		std::vector<GPUInfoCache> hardwareInfo;
	};

	struct ShaderBytecode
	{
		void* bytecode;
		size_t size;
	};

	enum WriteFlags
	{
		ColorR =	1 << 0,
		ColorG =	1 << 1,
		ColorB =	1 << 2,
		ColorA =	1 << 3,
		ColorRGB = ColorR | ColorG | ColorB,
		ColorRGBA = ColorRGB | ColorA,
	};

	enum class StencilOp
	{
		Keep,
		Zero,
		Replace,
		IncrSat,
		DcrSat,
		Invert,
		Incr,
		Dcr
	};

	enum class BlendValue
	{
		Zero,
		One,
		SrcColor,
		InvSrcColor,
		SrcAlpha,
		InvSrcAlpha,
		DestAlpha,
		InvDestAlpha,
		DestColor,
		InvDestColor
	};

	enum class BlendOp
	{
		Add,
		Subtract,
		Min,
		Max
	};

	enum class ComparisonOp
	{
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		GreaterEqual,
		NotEqual,
		Always
	};

	enum class CommandBufferType
	{
		Copy,
		Compute,
		Graphics,
	};

	enum class TopologyType
	{
		Undefined = 0,
		Point = 1,
		Line = 2,
		Triangle = 3,
		Patch = 4
	};

	enum class TopologyIndexMethod
	{
		Undefined = 0,
		Pointlist = 1,
		Linelist = 2,
		Linestrip = 3,
		Trianglelist = 4,
		Trianglestrip = 5,
	};

	enum class VertexWinding
	{
		CCW = 0,
		CW = 0,
	};

	enum class FillMode
	{
		Solid,
		Wireframe
	};

	enum class CullingMode
	{
		Backface = 0,
		Frontface = 0,
		None
	};

	enum class Topology
	{
		Point = 0,
		Line = 1,
		Triangle = 2,
		Patch = 3
	};

	enum class VertexElementFormat
	{
		Color,
		Half,
		Unorm,
		Short,
		Float,
		UInt,
	};

	enum class VertexElementUsage
	{
		Position,
		Texcoord,
		BlendIndices,
		BlendWeights,
		Normal,
		Tangent,
		Binormal,
	};

	struct StencilFace
	{
		StencilOp m_DepthPassOp;
		StencilOp m_StencilPassOp;
		StencilOp m_StencilFailOp;
		ComparisonOp m_StencilComparison;
	};

	struct RasterizerState
	{
		RasterizerState()
		{
			m_CullingMode = CullingMode::Backface;
			m_FillMode = FillMode::Solid;
			m_DepthBias = 0.05f;
			m_DepthBiasClamp = 0.0f;
			m_SampleCount = 1;
			m_ConservativeRasterizer = false;
		}

		CullingMode m_CullingMode;
		FillMode m_FillMode;
		float m_DepthBias;
		float m_DepthBiasClamp;
		bool m_MultisampleEnable;
		int m_SampleCount;
		bool m_ConservativeRasterizer;
	};

	struct DepthStencilState
	{
		DepthStencilState()
		{
			m_DepthEnable = true;
			m_StencilEnable = true;
			m_DepthOp = ComparisonOp::LessEqual;
			m_StencilReadMask = ColorRGBA;
			m_StencilWriteMask = ColorRGBA;
			m_StencilFrontFace = {};
			m_StencilBackFace = {};
		}

		bool m_DepthEnable;
		ComparisonOp m_DepthOp;
		bool m_StencilEnable;
		uint32_t m_StencilReadMask;
		uint32_t m_StencilWriteMask;
		StencilFace m_StencilFrontFace;
		StencilFace m_StencilBackFace;
	};

	struct RenderTargetBlendState
	{
		RenderTargetBlendState()
		{
			m_BlendEnable = true;
			m_ColorSrc = BlendValue::One;
			m_ColorDest = BlendValue::Zero;
			m_ColorOp = BlendOp::Add;
			m_AlphaSrc = BlendValue::One;
			m_AlphaDest = BlendValue::Zero;
			m_AlphaOp = BlendOp::Add;
			m_LogicOpEnable = false;
			m_WriteMask = ColorRGBA;
		}

		bool m_BlendEnable;
		bool m_LogicOpEnable;
		BlendValue m_ColorSrc;
		BlendValue m_ColorDest;
		BlendOp m_ColorOp;
		BlendValue m_AlphaSrc;
		BlendValue m_AlphaDest;
		BlendOp m_AlphaOp;
		uint32_t m_WriteMask;
	};

	struct BlendState
	{
		RenderTargetBlendState renderTargetBlendStates[8];
		bool alphaToCoverageEnable;
		bool independentBlendEnable;
	};

	enum class TextureFormat
	{
		TEXTURE_FORMAT_R32G32B32A32_FLOAT = 0,
		TEXTURE_FORMAT_R32G32B32A32_UINT = 1,
		TEXTURE_FORMAT_R32G32B32A32_SINT = 2,
		TEXTURE_FORMAT_R32G32B32_FLOAT = 3,
		TEXTURE_FORMAT_R32G32B32_UINT = 4,
		TEXTURE_FORMAT_R32G32B32_SINT = 5,
		TEXTURE_FORMAT_R16G16B16A16_FLOAT = 6,
		TEXTURE_FORMAT_R16G16B16A16_UNORM = 7,
		TEXTURE_FORMAT_R16G16B16A16_UINT = 8,
		TEXTURE_FORMAT_R16G16B16A16_SNORM = 9,
		TEXTURE_FORMAT_R16G16B16A16_SINT = 10,
		TEXTURE_FORMAT_R32G32_FLOAT = 11,
		TEXTURE_FORMAT_R32G32_UINT = 12,
		TEXTURE_FORMAT_R32G32_SINT = 13,
		TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT = 14,
		TEXTURE_FORMAT_R32_FLOAT_X8X24_TYPELESS = 15,
		TEXTURE_FORMAT_X32_TYPELESS_G8X24_UINT = 16,
		TEXTURE_FORMAT_R10G10B10A2_UNORM = 17,
		TEXTURE_FORMAT_R10G10B10A2_UINT = 18,
		TEXTURE_FORMAT_R11G11B10_FLOAT = 19,
		TEXTURE_FORMAT_R8G8B8A8_UNORM = 20,
		TEXTURE_FORMAT_R8G8B8A8_UNORM_SRGB = 21,
		TEXTURE_FORMAT_R8G8B8A8_UINT = 22,
		TEXTURE_FORMAT_R8G8B8A8_SNORM = 23,
		TEXTURE_FORMAT_R8G8B8A8_SINT = 24,
		TEXTURE_FORMAT_R16G16_FLOAT = 25,
		TEXTURE_FORMAT_R16G16_UNORM = 26,
		TEXTURE_FORMAT_R16G16_UINT = 27,
		TEXTURE_FORMAT_R16G16_SNORM = 28,
		TEXTURE_FORMAT_R16G16_SINT = 29,
		TEXTURE_FORMAT_D32_FLOAT = 30,
		TEXTURE_FORMAT_R32_FLOAT = 31,
		TEXTURE_FORMAT_R32_UINT = 32,
		TEXTURE_FORMAT_R32_SINT = 33,
		TEXTURE_FORMAT_D24_UNORM_S8_UINT = 34,
		TEXTURE_FORMAT_R16_FLOAT = 35,
		TEXTURE_FORMAT_D16_UNORM = 36,
		TEXTURE_FORMAT_R16_UNORM = 37,
		TEXTURE_FORMAT_R16_UINT = 38,
		TEXTURE_FORMAT_R16_SNORM = 39,
		TEXTURE_FORMAT_R16_SINT = 40,
		TEXTURE_FORMAT_R8_TYPELESS = 41,
		TEXTURE_FORMAT_R8_UNORM = 42,
		TEXTURE_FORMAT_R8_UINT = 43,
		TEXTURE_FORMAT_R8_SNORM = 44,
		TEXTURE_FORMAT_R8_SINT = 45,
		TEXTURE_FORMAT_INVALID = 46,
	};
}
