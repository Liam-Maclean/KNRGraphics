#pragma once
#include "shader.h"
#include "shader_cache.h"
#include <filesystem>

namespace KNR
{
	class OpenglShader
		: public Shader
	{
	public:
		OpenglShader(const std::filesystem::path& shader);
		virtual ~OpenglShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void* GetShaderBytecode() override;


	private:
		uint32_t m_rendererId = 0;

		std::vector<uint32_t> m_shaderBytecode{};
	};
}