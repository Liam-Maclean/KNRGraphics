#pragma once
#include "render_types.h"

/// <summary>
/// Shader
/// 
/// A singular shader input, read from file and compiled by KNRShaders.proj
/// Loads in a reflection file that displays constant buffers and register numbers
/// </summary>

namespace KNR
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual ShaderBytecode GetShaderBytecode() { return m_shaderBytecode; };
		inline const std::string GetName() { return m_name; }

		static Shader* Create(const char* shader);
	protected:
		ShaderBytecode m_shaderBytecode;
		std::string m_name;
	};
}