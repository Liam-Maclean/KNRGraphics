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

	enum class ConstantType
	{
		Constant_float,
		Constant_matrix,
		
	};

	struct ShaderInfo
	{
		std::string sourcePath;
		ShaderStage shaderStage;
	};

	struct ShaderConstant
	{
		std::string regName;
		uint32_t regNumber;
	};
	
	struct ShaderInspection
	{
		ShaderInfo shaderInfo;
		std::vector<ShaderConstant> shaderConstants;
	};
	

	class ShaderUtil
	{
	public:
		static ShaderBytecode LoadShaderBytecode(const ShaderInfo& info);
		static ShaderBytecode CompileHLSL(const ShaderInfo& info);
	};
}