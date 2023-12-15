#include "directx12_vertex_declaration.h"
#include "logger/logger.h"

namespace KNR
{
	//TODO
	VertexDeclaration* VertexDeclaration::Create(VertexAttribute* attributes, uint32_t count)
	{
		return new DirectX12VertexDeclaration(attributes, count);
	}

	DirectX12VertexDeclaration::DirectX12VertexDeclaration(VertexAttribute* attributes, uint32_t count)
	{

	}

	DirectX12VertexDeclaration::~DirectX12VertexDeclaration()
	{

	}
}

