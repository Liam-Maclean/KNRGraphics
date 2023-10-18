#pragma once

// STL
#include <string>			//std::string
#include <vector>			//std::vector
#include <unordered_map>	//std::unordered_map
#include <map>				//std::map
#include <cstdint>			//uint32_t, U64_t
#include <assert.h>			//assert
#include <utility>			//std::pair
#include <set>				//std::set

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef PLATFORM_DX12
#include <d3d12.h>			//Dx12 raw API
#include <dxgi1_5.h>		//DXGI library
#include <d3dcompiler.h>	//DirectX hlsl compiler (shaders) 
#endif

#ifdef PLATFORM_DX11
#include <d3d11>			//dx11 raw API
#endif

#ifdef PLATFORM_VULKAN
#include <vulkan/vulkan.h>	//Vulkan raw header
#endif

#ifdef PLATFORM_OPENGL
#include <gl/GL.h>			//Opengl
#include <gl/GLU.h>			//Opengl utilities
#endif