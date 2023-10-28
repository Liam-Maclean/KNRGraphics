#include "opengl_util.h"
#include "logger/logger.h"

namespace KNR
{
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


