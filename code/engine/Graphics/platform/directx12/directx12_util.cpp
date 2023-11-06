#include "directx12_util.h"
#include "logger/logger.h"

namespace KNR
{
	inline D3D12_FILL_MODE Util::ConvertFillMode(FillMode fillMode)
	{
		if (fillMode == FillMode::wireframe)
		{
			return D3D12_FILL_MODE_WIREFRAME;
		}
		else
		{
			return D3D12_FILL_MODE_SOLID;
		}
		return D3D12_FILL_MODE_SOLID;
	}

	inline D3D12_CULL_MODE Util::ConvertCullMode(CullingMode cullMode)
	{
		if (cullMode == CullingMode::backface)
		{
			return D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
		}
		else if (cullMode == CullingMode::frontface)
		{
			return D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT;
		}
		else
		{
			return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
		}
		return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
	}

	inline bool Util::IsDepthFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::TEXTURE_FORMAT_D32_FLOAT:
		case TextureFormat::TEXTURE_FORMAT_D24_UNORM_S8_UINT:
		case TextureFormat::TEXTURE_FORMAT_R32_UINT:
		case TextureFormat::TEXTURE_FORMAT_D16_UNORM:
			return true;
		}

		return false;
	}
}

