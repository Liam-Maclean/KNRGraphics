
#pragma once
#include "render_types.h"
namespace KNR
{
	namespace Util
	{
		static void ErrorCheck(VkResult result);
		static VkFormat GetVulkanFormatType(TextureFormat textureFormat);
		static bool IsDepthFormat(TextureFormat format);
	}
}