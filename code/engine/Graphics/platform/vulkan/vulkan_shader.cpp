#include <d3d12.h>
#include "platform/directx/directx_shader.h"
#include "platform/directx/directx_graphics_context.h"
#include "shader_util.h"
#include "shader_cache.h"

namespace KNR
{
	Shader* Shader::Create(const std::filesystem::path& shader)
	{
		return new DirectXShader(shader);
	}

	ShaderStage _ConvertExtensionToShaderStage(const std::string& extension)
	{
		if (strcmp(extension.c_str(), ".qvert") == 0)
		{
			return ShaderStage::vertex;
		}
		else if (strcmp(extension.c_str(), ".qfrag") == 0)
		{
			return ShaderStage::pixel;
		}
		else if (strcmp(extension.c_str(), ".qcomp") == 0)
		{
			return ShaderStage::compute;
		}
		
		//invalid extension
		assert(0);
		return ShaderStage::vertex;
	}

	DirectXShader::DirectXShader(const std::filesystem::path& shader)
	{
		//If the vertex src path exists
		if (!shader.empty())
		{
			ShaderInfo vertInfo = {};
			vertInfo.shaderStage = _ConvertExtensionToShaderStage(shader.extension().string());
			vertInfo.sourcePath = shader.string();

			ShaderCache vertCache;
			vertCache.Init(vertInfo.sourcePath);

			if (vertCache.ShaderNeedsCompiled() || true)
			{
				m_shaderBytecode = ShaderUtil::CompileHLSL(vertInfo);
				vertCache.WriteToFile(m_shaderBytecode);
			}
			else
			{
				m_shaderBytecode = vertCache.ReadFromFile();
			}

			m_pipelineShaderBytecode.BytecodeLength = (m_shaderBytecode.size() * sizeof(uint32_t));
			m_pipelineShaderBytecode.pShaderBytecode = m_shaderBytecode.data();
		}
	}



	DirectXShader::~DirectXShader()
	{

	}

	void DirectXShader::Bind() const
	{

	}

	void DirectXShader::Unbind() const
	{
		
	}

	void* DirectXShader::GetShaderBytecode()
	{
		return static_cast<void*>(&m_pipelineShaderBytecode);
	}
}