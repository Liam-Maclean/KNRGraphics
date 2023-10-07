#pragma once
#include "shader.h"
#include "shader_cache.h"

struct D3D12_SHADER_BYTECODE;
namespace KNR
{
	class DirectXShader
		: public Shader
	{
	public:
		DirectXShader(const std::filesystem::path& shader);
		virtual ~DirectXShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void* GetShaderBytecode() override;


	private:
		uint32_t m_rendererId = 0;
		D3D12_SHADER_BYTECODE m_pipelineShaderBytecode;

		std::vector<uint32_t> m_shaderBytecode{};
	};
}