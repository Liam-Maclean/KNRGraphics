#include "qlpch.h"
#include <d3d12.h>
#include "graphics/platform/directx/directx_shader.h"
#include "graphics/platform/directx/directx_graphics_context.h"
#include "graphics/shader_util.h"
#include "graphics/shader_cache.h"
//#include  "../dxc/dxcapi.h"
#include "../dxcTest/inc/dxcapi.h"
#include <logging/log.h>
#include <d3dcompiler.h>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <array>

namespace QRender
{
	Ref<Shader> Shader::Create(const std::filesystem::path& shader)
	{
		return MakeRef<DirectXShader>(shader);
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
				Q_RENDER_INFO("COMPILING SHADER: {0}", shader.string());
				m_shaderBytecode = ShaderUtil::CompileHLSL(vertInfo);
				vertCache.WriteToFile(m_shaderBytecode);
			}
			else
			{
				Q_RENDER_INFO("SHADER ALREADY EXISTS, GRABBING FROM CACHE: {0}", shader.string());
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
