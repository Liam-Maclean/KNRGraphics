#pragma once

#include "render_types.h"
#include "vertex.h"
#include "buffer.h"
namespace KNR
{
	class StaticMesh
	{
	public:
		StaticMesh() {};
		uint32_t indexCount;
		uint32_t vertexCount;
		std::vector<uint32_t> indices;
	};
}