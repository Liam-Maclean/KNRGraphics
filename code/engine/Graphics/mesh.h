#pragma once

#include "graphics/render_types.h"
#include "material.h"
#include "vertex.h"
#include "buffer.h"
namespace QRender
{
	class StaticMesh
	{
	public:
		StaticMesh() {};
		uint32_t indexCount;
		uint32_t vertexCount;
		std::vector<uint32_t> indices;
		std::vector<PrimitiveVertex> vertices;
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(StaticMesh, indexCount, vertexCount, indices, vertices)
	};
}