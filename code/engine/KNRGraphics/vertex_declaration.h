#pragma once
#include "render_types.h"

/// <summary>
/// Vertex Declaration
/// 
/// A collection of vertex attribute descriptions making up a single vertices
/// Pass in an array of attributes. Needed for creating vertex buffers and must 
/// Match shader vertex inputs
/// </summary>

namespace KNR
{
	class VertexDeclaration
	{
	public:
		static VertexDeclaration* Create(VertexAttribute* attributes, uint32_t count);
		virtual ~VertexDeclaration() = default;
		uint32_t GetVertexStride() { return m_AttributesStride; }
	private:
		uint32_t m_AttributesCount;
		uint32_t m_AttributesStride;
	};
}