#include "directx12_util.h"
#include "logger/logger.h"

namespace KNR
{
	inline D3D12_FILL_MODE Util::ConvertFillMode(FillMode fillMode)
	{
		if (fillMode == FillMode::Wireframe)
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
		if (cullMode == CullingMode::Backface)
		{
			return D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
		}
		else if (cullMode == CullingMode::Frontface)
		{
			return D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT;
		}
		else
		{
			return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
		}
		return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
	}

	inline D3D12_BLEND Util::ConvertBlendMode(BlendValue blendValue)
	{
		switch (blendValue)
		{
		case BlendValue::One:
			return D3D12_BLEND_ONE;
			break;
		case BlendValue::Zero:
			return D3D12_BLEND_ZERO;
			break;
		case BlendValue::SrcColor:
			return D3D12_BLEND_SRC_COLOR;
			break;
		case BlendValue::InvSrcColor:
			return D3D12_BLEND_INV_SRC_COLOR;
			break;
		case BlendValue::SrcAlpha:
			return D3D12_BLEND_SRC_ALPHA;
			break;
		case BlendValue::InvSrcAlpha:
			return D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case BlendValue::DestAlpha:
			return D3D12_BLEND_DEST_ALPHA;
			break;
		case BlendValue::InvDestAlpha:
			return D3D12_BLEND_INV_DEST_ALPHA;
			break;
		case BlendValue::DestColor:
			return D3D12_BLEND_DEST_COLOR;
			break;	
		case BlendValue::InvDestColor:
			return D3D12_BLEND_INV_DEST_COLOR;
			break;
		}
		KNT_ERROR("INVALID BLEND MODE");
		return D3D12_BLEND();
	}

	inline D3D12_BLEND_OP Util::ConvertBlendOp(BlendOp blendOp)
	{
		switch (blendOp)
		{
		case BlendOp::Add:
			return D3D12_BLEND_OP_ADD;
			break;
		case BlendOp::Subtract:
			return D3D12_BLEND_OP_SUBTRACT;
			break;
		case BlendOp::Min:
			return D3D12_BLEND_OP_MIN;
			break;
		case BlendOp::Max:
			return D3D12_BLEND_OP_MAX;
			break;
		}
		KNT_ERROR("INVALID BLEND OP");
		return D3D12_BLEND_OP();
	}

	inline D3D12_COMPARISON_FUNC Util::ConvertCompareOp(ComparisonOp comparison)
	{
		switch (comparison)
		{
		case ComparisonOp::Never:
			return D3D12_COMPARISON_FUNC_NEVER;
			break;
		case ComparisonOp::Less:
			return D3D12_COMPARISON_FUNC_LESS;
			break;
		case ComparisonOp::Equal:
			return D3D12_COMPARISON_FUNC_EQUAL;
			break;
		case ComparisonOp::LessEqual:
			return D3D12_COMPARISON_FUNC_LESS_EQUAL;
			break;
		case ComparisonOp::Greater:
			return D3D12_COMPARISON_FUNC_GREATER;
			break;
		case ComparisonOp::GreaterEqual:
			return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			break;
		case ComparisonOp::NotEqual:
			return D3D12_COMPARISON_FUNC_NOT_EQUAL;
			break;
		case ComparisonOp::Always:
			return D3D12_COMPARISON_FUNC_ALWAYS;
			break;
		}
		KNT_ERROR("INVALID COMPARISON OP");
		return D3D12_COMPARISON_FUNC();
	}

	inline D3D12_SHADER_BYTECODE Util::ConvertBytecode(ShaderBytecode bytecode)
	{
		D3D12_SHADER_BYTECODE shaderBytecode;
		shaderBytecode.BytecodeLength = bytecode.size;
		shaderBytecode.pShaderBytecode = bytecode.bytecode;
		return shaderBytecode;
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

	DXGI_FORMAT Util::GetDXGIFormatType(TextureFormat textureFormat)
	{
		switch (textureFormat)
		{
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32A32_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32A32_UINT:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32A32_SINT:
			return DXGI_FORMAT_R32G32B32A32_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32_FLOAT:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32_UINT:
			return DXGI_FORMAT_R32G32B32_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32_SINT:
			return DXGI_FORMAT_R32G32B32_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_FLOAT:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_UNORM:
			return DXGI_FORMAT_R16G16B16A16_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_UINT:
			return DXGI_FORMAT_R16G16B16A16_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_SNORM:
			return DXGI_FORMAT_R16G16B16A16_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_SINT:
			return DXGI_FORMAT_R16G16B16A16_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32_UINT:
			return DXGI_FORMAT_R32G32_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32_SINT:
			return DXGI_FORMAT_R32G32_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		case KNR::TextureFormat::TEXTURE_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R10G10B10A2_UNORM:
			return DXGI_FORMAT_R10G10B10A2_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R10G10B10A2_UINT:
			return DXGI_FORMAT_R10G10B10A2_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R11G11B10_FLOAT:
			return DXGI_FORMAT_R11G11B10_FLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_UINT:
			return DXGI_FORMAT_R8G8B8A8_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_SNORM:
			return DXGI_FORMAT_R8G8B8A8_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_SINT:
			return DXGI_FORMAT_R8G8B8A8_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_FLOAT:
			return DXGI_FORMAT_R16G16_FLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_UNORM:
			return DXGI_FORMAT_R16G16_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_UINT:
			return DXGI_FORMAT_R16G16_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_SNORM:
			return DXGI_FORMAT_R16G16_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_SINT:
			return DXGI_FORMAT_R16G16_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_D32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32_UINT:
			return DXGI_FORMAT_R32_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32_SINT:
			return DXGI_FORMAT_R32_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_D24_UNORM_S8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_D16_UNORM:
			return DXGI_FORMAT_D16_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16_UNORM:
			return DXGI_FORMAT_R16_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16_UINT:
			return DXGI_FORMAT_R16_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16_SNORM:
			return DXGI_FORMAT_R16_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16_SINT:
			return DXGI_FORMAT_R16_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8_UNORM:
			return DXGI_FORMAT_R8_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8_UINT:
			return DXGI_FORMAT_R8_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8_SNORM:
			return DXGI_FORMAT_R8_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8_SINT:
			return DXGI_FORMAT_R8_SINT;
		default:
			break;
		}

		KNT_ERROR("Unknown Texture Format, Please select a valid format");
		//Unknown texture format
		assert(0);
	}
}

