#pragma once
#include "render_types.h"
#include "vertex_declaration.h"
namespace KNR
{
	class DirectX12VertexDeclaration 
		: public VertexDeclaration
	{
	public:
		DirectX12VertexDeclaration(VertexAttribute* attributes, uint32_t count);
		~DirectX12VertexDeclaration();

		inline D3D12_INPUT_LAYOUT_DESC GetVertexInputLayout() { return m_VertexInputLayout; }
	private:
		D3D12_INPUT_LAYOUT_DESC m_VertexInputLayout;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_VertexAttributes;
	};
}