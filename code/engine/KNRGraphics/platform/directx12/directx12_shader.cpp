#include <d3d12.h>
#include "directx12_shader.h"
#include "directx12_graphics_context.h"
#include "utility/file_system.h"
#include <filesystem>

namespace KNR
{
	//TODO
	Shader* Shader::Create(const char* shader)
	{
		return new DirectX12Shader(shader);
	}

	//TODO - Introspection file that also reads in reflection information
	DirectX12Shader::DirectX12Shader(const char* shader)
	{
		std::vector<uint32_t> fileBytecode;
		fileBytecode = LightweightFileSystem::LoadFile<uint32_t>(shader);
		m_shaderBytecode.bytecode = fileBytecode.data();
		m_shaderBytecode.size = fileBytecode.size();
	}

	DirectX12Shader::~DirectX12Shader()
	{

	}
}
