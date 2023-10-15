#include "vulkan_util.h"

namespace KNR
{
	namespace Util
	{
		static void ErrorCheck(VkResult result)
		{
			if (result < 0)
			{
				switch (result)
				{
				case VK_ERROR_OUT_OF_HOST_MEMORY:
					//std::cout << "VK_ERROR_OUT_OF_HOST_MEMORY" << std::endl;
					break;
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:
					//std::cout << "VK_ERROR_OUT_OF_DEVICE_MEMORY" << std::endl;
					break;
				case VK_ERROR_INITIALIZATION_FAILED:
					//std::cout << "VK_ERROR_INITIALIZATION_FAILED" << std::endl;
					break;
				case VK_ERROR_DEVICE_LOST:
					//std::cout << "VK_ERROR_DEVICE_LOST" << std::endl;
					break;
				case VK_ERROR_MEMORY_MAP_FAILED:
					//std::cout << "VK_ERROR_MEMORY_MAP_FAILED" << std::endl;
					break;
				case VK_ERROR_LAYER_NOT_PRESENT:
					//std::cout << "VK_ERROR_LAYER_NOT_PRESENT" << std::endl;
					break;
				case VK_ERROR_EXTENSION_NOT_PRESENT:
					//std::cout << "VK_ERROR_EXTENSION_NOT_PRESENT" << std::endl;
					break;
				case VK_ERROR_FEATURE_NOT_PRESENT:
					//std::cout << "VK_ERROR_FEATURE_NOT_PRESENT" << std::endl;
					break;
				case VK_ERROR_INCOMPATIBLE_DRIVER:
					//std::cout << "VK_ERROR_INCOMPATIBLE_DRIVER" << std::endl;
					break;
				case VK_ERROR_TOO_MANY_OBJECTS:
					//std::cout << "VK_ERROR_TOO_MANY_OBJECTS" << std::endl;
					break;
				case VK_ERROR_FORMAT_NOT_SUPPORTED:
					//std::cout << "VK_ERROR_FORMAT_NOT_SUPPORTED" << std::endl;
					break;
				case VK_ERROR_FRAGMENTED_POOL:
					//std::cout << "VK_ERROR_FRAGMENTED_POOL" << std::endl;
					break;
				case VK_ERROR_OUT_OF_POOL_MEMORY:
					//std::cout << "VK_ERROR_OUT_OF_POOL_MEMORY" << std::endl;
					break;
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:
					//std::cout << "VK_ERROR_INVALID_EXTERNAL_HANDLE" << std::endl;
					break;
				case VK_ERROR_SURFACE_LOST_KHR:
					//std::cout << "VK_ERROR_SURFACE_LOST_KHR" << std::endl;
					break;
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
					//std::cout << "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR" << std::endl;
					break;
				case VK_SUBOPTIMAL_KHR:
					//std::cout << "VK_SUBOPTIMAL_KHR" << std::endl;
					break;
				case VK_ERROR_OUT_OF_DATE_KHR:
					//std::cout << "VK_ERROR_OUT_OF_DATE_KHR" << std::endl;
					break;
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
					//std::cout << "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR" << std::endl;
					break;
				case VK_ERROR_VALIDATION_FAILED_EXT:
					//std::cout << "VK_ERROR_VALIDATION_FAILED_EXT" << std::endl;
					break;
				default:
					break;
				}
				assert(0 && "Vulkan runtime error.");
			}
		}

		VkFormat GetVulkanFormatType(TextureFormat textureFormat)
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


			//Unknown texture format
			assert(0);
		}
	}
}

