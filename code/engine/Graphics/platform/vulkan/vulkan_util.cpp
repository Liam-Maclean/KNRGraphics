#include "vulkan_util.h"
#include "logger/logger.h"

namespace KNR
{
	void Util::ErrorCheck(VkResult result)
	{
		if (result < 0)
		{
			switch (result)
			{
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				KNT_ERROR("VK_ERROR_OUT_OF_HOST_MEMORY");
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				KNT_ERROR("VK_ERROR_OUT_OF_DEVICE_MEMORY");
				break;
			case VK_ERROR_INITIALIZATION_FAILED:
				KNT_ERROR("VK_ERROR_INITIALIZATION_FAILED");
				break;
			case VK_ERROR_DEVICE_LOST:
				KNT_ERROR("VK_ERROR_DEVICE_LOST");
				break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				KNT_ERROR("VK_ERROR_MEMORY_MAP_FAILED");
				break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				KNT_ERROR("VK_ERROR_LAYER_NOT_PRESENT");
				break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				KNT_ERROR("VK_ERROR_EXTENSION_NOT_PRESENT");
				break;
			case VK_ERROR_FEATURE_NOT_PRESENT:
				KNT_ERROR("VK_ERROR_FEATURE_NOT_PRESENT");
				break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				KNT_ERROR("VK_ERROR_INCOMPATIBLE_DRIVER");
				break;
			case VK_ERROR_TOO_MANY_OBJECTS:
				KNT_ERROR("VK_ERROR_TOO_MANY_OBJECTS");
				break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				KNT_ERROR("VK_ERROR_FORMAT_NOT_SUPPORTED");
				break;
			case VK_ERROR_FRAGMENTED_POOL:
				KNT_ERROR("VK_ERROR_FRAGMENTED_POOL");
				break;
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				KNT_ERROR("VK_ERROR_OUT_OF_POOL_MEMORY");
				break;
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				KNT_ERROR("VK_ERROR_INVALID_EXTERNAL_HANDLE");
				break;
			case VK_ERROR_SURFACE_LOST_KHR:
				KNT_ERROR("VK_ERROR_SURFACE_LOST_KHR");
				break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				KNT_ERROR("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");
				break;
			case VK_SUBOPTIMAL_KHR:
				KNT_ERROR("VK_SUBOPTIMAL_KHR");
				break;
			case VK_ERROR_OUT_OF_DATE_KHR:
				KNT_ERROR("VK_ERROR_OUT_OF_DATE_KHR");
				break;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				KNT_ERROR("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
				break;
			case VK_ERROR_VALIDATION_FAILED_EXT:
				KNT_ERROR("VK_ERROR_VALIDATION_FAILED_EXT");
				break;
			default:
				break;
			}
			KNT_ERROR("Vulkan runtime error");
			assert(0);
		}
	}

	VkFormat Util::GetVulkanFormatType(TextureFormat textureFormat)
	{
		switch (textureFormat)
		{
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32A32_FLOAT:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32A32_UINT:
			return VK_FORMAT_R32G32B32A32_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32A32_SINT:
			return VK_FORMAT_R32G32B32A32_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32_FLOAT:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32_UINT:
			return VK_FORMAT_R32G32B32_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32B32_SINT:
			return VK_FORMAT_R32G32B32_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_FLOAT:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_UNORM:
			return VK_FORMAT_R16G16B16A16_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_UINT:
			return VK_FORMAT_R16G16B16A16_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_SNORM:
			return VK_FORMAT_R16G16B16A16_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16B16A16_SINT:
			return VK_FORMAT_R16G16B16A16_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32_FLOAT:
			return VK_FORMAT_R32G32_SFLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32_UINT:
			return VK_FORMAT_R32G32_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32G32_SINT:
			return VK_FORMAT_R32G32_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_X32_TYPELESS_G8X24_UINT:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R10G10B10A2_UNORM:
			return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
		case KNR::TextureFormat::TEXTURE_FORMAT_R10G10B10A2_UINT:
			return VK_FORMAT_A2R10G10B10_UINT_PACK32;
		case KNR::TextureFormat::TEXTURE_FORMAT_R11G11B10_FLOAT:
			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_UNORM_SRGB:
			return VK_FORMAT_B8G8R8A8_SRGB;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_UINT:
			return VK_FORMAT_B8G8R8A8_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_SNORM:
			return VK_FORMAT_B8G8R8A8_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8G8B8A8_SINT:
			return VK_FORMAT_B8G8R8A8_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_FLOAT:
			return VK_FORMAT_R16G16_SFLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_UNORM:
			return VK_FORMAT_R16G16_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_UINT:
			return VK_FORMAT_R16G16_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_SNORM:
			return VK_FORMAT_R16G16_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16G16_SINT:
			return VK_FORMAT_R16G16_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_D32_FLOAT:
			return VK_FORMAT_D32_SFLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32_FLOAT:
			return VK_FORMAT_R32_SFLOAT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32_UINT:
			return VK_FORMAT_R32_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R32_SINT:
			return VK_FORMAT_R32_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_D24_UNORM_S8_UINT:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_D16_UNORM:
			return VK_FORMAT_D16_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16_UNORM:
			return VK_FORMAT_R16_UNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16_UINT:
			return VK_FORMAT_R16_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16_SNORM:
			return VK_FORMAT_R16_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R16_SINT:
			return VK_FORMAT_R16_SINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8_UNORM:
			return VK_FORMAT_R8_SSCALED;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8_UINT:
			return VK_FORMAT_R8_UINT;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8_SNORM:
			return VK_FORMAT_R8_SNORM;
		case KNR::TextureFormat::TEXTURE_FORMAT_R8_SINT:
			return VK_FORMAT_R8_SINT;
		default:
			break;
		}

		KNT_ERROR("Unknown Texture Format, Please select a valid format");
		//Unknown texture format
		assert(0);
	}

	bool Util::IsDepthFormat(TextureFormat format)
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

