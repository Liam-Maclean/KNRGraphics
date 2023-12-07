#pragma once
#include "render_types.h"

//Forward declare
namespace std
{
	namespace filesystem
	{
		class path;
	}
}

namespace KNR
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual ShaderBytecode GetShaderBytecode() { return m_shaderBytecode; };
		inline const std::string GetName() { return m_name; }

		static Shader* Create(const std::filesystem::path& shader);
	protected:
		ShaderBytecode m_shaderBytecode;
		std::string m_name;
	};
}