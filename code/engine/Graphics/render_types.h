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

	enum class DepthComparisonOp
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
		copy,
		compute,
		graphics,
	};

	enum class TopologyType
	{
		undefined = 0,
		point = 1,
		line = 2,
		triangle = 3,
		patch = 4
	};

	enum class TopologyIndexMethod
	{
		undefined = 0,
		pointlist = 1,
		linelist = 2,
		linestrip = 3,
		trianglelist = 4,
		trianglestrip = 5,
	};

	enum class VertexWinding
	{
		CCW = 0,
		CW = 0,
	};

	enum class FillMode
	{
		solid,
		wireframe
	};

	enum class CullingMode
	{
		backface = 0,
		frontface = 0,
		none
	};

	enum class Topology
	{
		point = 0,
		line = 1,
		triangle = 2,
		patch = 3
	};

	enum class VertexElementFormat
	{
		Byte4,
		Color,
		Half,
		Unorm,
		Short,
		Float,
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
