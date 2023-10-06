#pragma once
#include "core/core.h"
#include <vector>

namespace QRender
{
	enum class ShaderStage
	{
		pixel,
		vertex,
		domain,
		hull,
		compute
	};

	struct ShaderInfo
	{
		std::string sourcePath;
		ShaderStage shaderStage;
	};

	class ShaderUtil
	{
	public:
		static std::vector<uint32_t> CompileHLSL(ShaderInfo info);
	};
}