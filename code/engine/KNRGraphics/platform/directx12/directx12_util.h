#pragma once
#include "render_types.h"
#include <d3d12.h>
#include <dxgi1_3.h>
namespace KNR
{
	class Util final
	{
	public:
		static DXGI_FORMAT GetDXGIFormatType(TextureFormat textureFormat);
		static DXGI_FORMAT GetDXGITypelessFormat(TextureFormat textureFormat);
		static DXGI_FORMAT GetDXGIFormatFromVertexDeclaration(VertexAttributeFormat format, uint32_t count);
		static D3D12_FILL_MODE ConvertFillMode(FillMode fillMode);
		static D3D12_CULL_MODE ConvertCullMode(CullingMode cullMode);
		static D3D12_BLEND ConvertBlendMode(BlendValue cullMode);
		static D3D12_BLEND_OP ConvertBlendOp(BlendOp cullMode);
		static D3D12_COMPARISON_FUNC ConvertCompareOp(ComparisonOp cullMode);
		static D3D12_SHADER_BYTECODE ConvertBytecode(ShaderBytecode bytecode);
		static LPCSTR ConvertVertexUsage(VertexAttributeUsage usage);
		static bool IsDepthFormat(TextureFormat format);
	};
}