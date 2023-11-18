#pragma once
#include "render_types.h"
namespace KNR
{
	class Util final
	{
	public:
		//static void ErrorCheck(VkResult result);
		static DXGI_FORMAT GetDXGIFormatType(TextureFormat textureFormat);
		inline static D3D12_FILL_MODE ConvertFillMode(FillMode fillMode);
		inline static D3D12_CULL_MODE ConvertCullMode(CullingMode cullMode);
		inline static bool IsDepthFormat(TextureFormat format);
	};
}