#pragma once

#include "core/core.h"
#include "logging/log.h"

namespace QRender
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		std::string GetName() { return m_name; }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void* GetShaderBytecode() = 0;

		static Ref<Shader> Create(const std::filesystem::path& shader);
	protected:
		std::string m_name;
	};
}