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

		VkFormat GetVulkanFormatFromGenericType(FramebufferTextureFormat textureFormat)
		{
			switch (textureFormat)
			{
			case  FramebufferTextureFormat::RGBA8:
				imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
				break;
			case FramebufferTextureFormat::RG16:
				imageCreateInfo.format = VK_FORMAT_R16G16_SFLOAT;
				break;
			case  FramebufferTextureFormat::RED_INTEGER:
				imageCreateInfo.format = VK_FORMAT_R32_SFLOAT;
				break;
			default:
				//UNEXPECTED FORMAT
				break;
			}

			return VkFormat();
		}
	}
}

