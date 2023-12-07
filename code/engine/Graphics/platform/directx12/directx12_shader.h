#pragma once
#include "render_types.h"
#include "shader.h"
#include "shader_cache.h"

struct D3D12_SHADER_BYTECODE;
namespace KNR
{
	class DirectX12Shader
		: public Shader
	{
	public:
		DirectX12Shader(const std::filesystem::path& shader);
		virtual ~DirectX12Shader();
	};
}