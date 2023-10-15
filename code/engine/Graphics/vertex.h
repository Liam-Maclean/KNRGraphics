 #pragma once
namespace KNR
{
	enum VertexLayout
	{
		gltfVertexLayout,
		quadVertexLayout,
		primitiveVertexLayout,
	};

	struct VertexPosition
	{
		Vector3f	Position		= { 0,0,0 };
	};

	struct VertexPositionColour 
		: public VertexPosition
	{
		Vector4f	Color			= { 1,1,1,1 };
	};

	struct VertexPositionColourTexcoord 
		: public VertexPositionColour
	{
		Vector2f	TexCoord		= { 0,0 };
		float		TexIndex		= { 0.0f };
	};

	struct Vertex
	{
		Vector3f	Position		= { 0,0,0 };
		Vector3f	Normal			= { 0,1,0 };
		Vector4f	Color			= { 1,1,1,1 };
		Vector2f	TexCoord		= { 0,0 };
		float		TexIndex		= { 0.0f };
		float		TilingFactor	= { 1.0f };
	};

	struct TestVertex
	{
		Vector3f	Position = { 0,0,0 };
		Vector4f	Color = { 1,1,1,1 };
		Vector2f	TexCoord = { 0,0 };
		float		TexIndex = { 0.0f };
	};

	struct PrimitiveVertex
	{
		Vector4f Position = { 0, 0, 0, 1 }; //0
		Vector4f Normal ={ 0, 0, 0, 1 }; //16
		Vector2f TexCoord = { 0, 0 }; //32
		Vector4f DebugColor = { 0, 0, 0, 1 }; //46
		//44
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(PrimitiveVertex, Position, Normal, TexCoord, DebugColor)
		const inline bool operator==(const KNR::PrimitiveVertex& a) const
		{
			return (a.Position == Position
				&& a.Normal == Normal
				&& a.TexCoord == TexCoord);
		}
	};

	struct QuadVertex
	{
		//Default constructor
		QuadVertex() : position(0, 0, 0), normal(0, 0, 0), uv(0, 0) {};
		//Override constructor
		QuadVertex(float posX, float posY, float posZ, float normX, float normY, float normZ, float u, float v)
			: position(posX, posY, posZ), normal(normX, normY, normZ), uv(u, v) {}
		Vector3f position;
		Vector3f normal;
		Vector2f uv;
	};

	struct gltfVertex
	{
		Vector3f pos;
		Vector3f normal;
		Vector2f uv0;
		Vector2f uv1;
		Vector4f joint0;
		Vector4f weight0;
	};
}