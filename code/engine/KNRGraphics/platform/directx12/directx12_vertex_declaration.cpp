#include "directx12_vertex_declaration.h"
#include "directx12_util.h"
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
		for (size_t i = 0; i < count; ++i)
		{
			D3D12_INPUT_ELEMENT_DESC inputElementDesc = {};
			inputElementDesc.Format = Util::GetDXGIFormatFromVertexDeclaration(attributes[i].m_Format, attributes[i].m_AttributeCount);
			inputElementDesc.SemanticName = Util::ConvertVertexUsage(attributes[i].m_Usage);
			inputElementDesc.SemanticIndex = 0;
			inputElementDesc.AlignedByteOffset = 0;
			inputElementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			inputElementDesc.InputSlot = i;

			m_VertexAttributes.push_back(inputElementDesc);
		}
		m_VertexInputLayout.NumElements = m_VertexAttributes.size();
		m_VertexInputLayout.pInputElementDescs = m_VertexAttributes.data();
	}

	DirectX12VertexDeclaration::~DirectX12VertexDeclaration()
	{

	}
}

