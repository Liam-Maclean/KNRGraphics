
#pragma once
#include "render_types.h"
namespace KNR
{
	class Util
	{
	public:
		static void ErrorCheck(VkResult result);
		static VkFormat GetVulkanFormatType(TextureFormat textureFormat);
		static bool IsDepthFormat(TextureFormat format);
	};
}