#pragma once

namespace QRender
{
	enum class TopologyType
	{
		undefined = 0,
		point = 1,
		line = 2,
		triangle = 3,
		patch = 4
	};

	enum class TopologyIndexMethod
	{
		undefined = 0,
		pointlist = 1,
		linelist = 2,
		linestrip = 3,
		trianglelist = 4,
		trianglestrip = 5,
	};

	enum class VertexWinding
	{
		CCW = 0,
		CW = 0,
	};

	enum class CullingMode
	{
		backface = 0,
		frontface = 0,
		none
	};
}
