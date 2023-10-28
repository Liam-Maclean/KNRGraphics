#pragma once
#include <d3d12.h>
namespace KNR
{
	class Technique;
	class DirectX12IndirectSignature
	{
	public:
		DirectX12IndirectSignature();
		~DirectX12IndirectSignature();

		void CreateSignature(Technique* technique, uint32_t byteStride);
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