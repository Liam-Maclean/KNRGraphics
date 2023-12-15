#include "directx11_shader.h"
#include "directx11_graphics_context.h"
#include "shader_util.h"
#include "shader_cache.h"

namespace KNR
{
	Shader* Shader::Create(const std::filesystem::path& shader)
	{
		return new DirectX11Shader(shader);
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

	DirectX11Shader::DirectX11Shader(const std::filesystem::path& shader)
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
		}
	}

	DirectX11Shader::~DirectX11Shader()
	{

	}

	void DirectX11Shader::Bind() const
	{

	}

	void DirectX11Shader::Unbind() const
	{
		
	}

	void* DirectX11Shader::GetShaderBytecode()
	{
		return nullptr;
	}
}
