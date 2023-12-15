#pragma once
#include "render_types.h"
namespace KNR
{
	class Util final
	{
	public:
		//static void ErrorCheck(VkResult result);
		inline static DXGI_FORMAT GetDXGIFormatType(TextureFormat textureFormat);
		inline static D3D12_FILL_MODE ConvertFillMode(FillMode fillMode);
		inline static D3D12_CULL_MODE ConvertCullMode(CullingMode cullMode);
		inline static D3D12_BLEND ConvertBlendMode(BlendValue cullMode);
		inline static D3D12_BLEND_OP ConvertBlendOp(BlendOp cullMode);
		inline static D3D12_COMPARISON_FUNC ConvertCompareOp(ComparisonOp cullMode);
		inline static D3D12_SHADER_BYTECODE ConvertBytecode(ShaderBytecode bytecode);
		inline static bool IsDepthFormat(TextureFormat format);
	};
}