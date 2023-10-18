#pragma once
#include <filesystem>
namespace KNR
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		std::string GetName() { return m_name; }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void* GetShaderBytecode() = 0;

		static Shader* Create(const std::filesystem::path& shader);
	protected:
		std::string m_name;
	};
}