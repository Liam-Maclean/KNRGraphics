#pragma once
#include "shader.h"
#include "shader_cache.h"

namespace KNR
{
	class VulkanShader
		: public Shader
	{
	public:
		VulkanShader(const std::filesystem::path& shader);
		virtual ~VulkanShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void* GetShaderBytecode() override;


	private:
		uint32_t m_rendererId = 0;

		std::vector<uint32_t> m_shaderBytecode{};
	};
}