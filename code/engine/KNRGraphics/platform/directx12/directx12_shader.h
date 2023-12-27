#pragma once
#include "render_types.h"
#include "shader.h"

namespace KNR
{
	class DirectX12Shader
		: public Shader
	{
	public:
		DirectX12Shader(const char* shader);
		virtual ~DirectX12Shader();
	};
}