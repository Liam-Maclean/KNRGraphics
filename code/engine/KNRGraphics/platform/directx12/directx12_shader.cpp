#include <d3d12.h>
#include "directx12_shader.h"
#include "directx12_graphics_context.h"
#include <filesystem>
namespace KNR
{
	//TODO
	Shader* Shader::Create(const std::filesystem::path& shader)
	{
		return new DirectX12Shader(shader);
	}

	DirectX12Shader::DirectX12Shader(const std::filesystem::path& shader)
	{
	
	}

	DirectX12Shader::~DirectX12Shader()
	{

	}
}
