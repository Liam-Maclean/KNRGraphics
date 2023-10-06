#pragma once
#include "utilities/callback.h"
#include <d3d12.h>
namespace QRender
{
	class Technique;
	class IndirectSignature
	{
	public:
		IndirectSignature();
		~IndirectSignature();

		void CreateSignature(Ref<Technique> technique, uint32_t byteStride);
		void AddIndirectConstantBufferViewArg(uint32_t rootParameterIndex);
		void AddIndirectConstantArg(uint32_t rootParameterIndex, uint32_t destOffset, uint32_t numValuesToSet);
		void AddIndirectShaderResourceViewArg(uint32_t rootParameterIndex);
		void AddIndirectDrawArg();
		ID3D12CommandSignature* GetCommandSignature() { return m_commandSignature; }
	private:
		ID3D12CommandSignature* m_commandSignature;
		std::vector<D3D12_INDIRECT_ARGUMENT_DESC> m_indirectArgs;
	protected:
	};
}