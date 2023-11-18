#pragma once
#include <vector>
#include "render_types.h"
namespace KNR
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
		static ShaderBytecode CompileHLSL(const ShaderInfo& info);
	};
}