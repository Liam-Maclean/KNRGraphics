#include "directx12_indirect_signature.h"
#include "directx12_graphics_context.h"
#include "directx12_technique.h"

namespace KNR
{
	DirectX12IndirectSignature::DirectX12IndirectSignature()
	{
      
	}

	DirectX12IndirectSignature::~DirectX12IndirectSignature()
	{

	}

    void DirectX12IndirectSignature::CreateSignature(Technique* technique, const uint32_t byteStride)
    {
        ID3D12Device* device = DirectX12Context.GetDevice();
        HRESULT hr;
        // Each command consists of a CBV update and a DrawInstanced call.
        D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
        commandSignatureDesc.pArgumentDescs = m_indirectArgs.data();
        commandSignatureDesc.NumArgumentDescs = static_cast<uint32_t>(m_indirectArgs.size());
        commandSignatureDesc.ByteStride = byteStride;
        hr = device->CreateCommandSignature(&commandSignatureDesc, technique ? static_cast<DirectX12Technique*>(technique)->GetRootSignature() : nullptr, IID_PPV_ARGS(&m_commandSignature));
        if (FAILED(hr))
        {
            assert(false);
        }
    }

    void DirectX12IndirectSignature::AddIndirectConstantBufferViewArg(const uint32_t rootParameterIndex)
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
        argumentDesc.ConstantBufferView.RootParameterIndex = rootParameterIndex;
        m_indirectArgs.push_back(argumentDesc);
    }

    void DirectX12IndirectSignature::AddIndirectConstantArg(const uint32_t rootParameterIndex, const uint32_t destOffset, const uint32_t numValuesToSet)
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
        argumentDesc.Constant.RootParameterIndex = rootParameterIndex;
        argumentDesc.Constant.DestOffsetIn32BitValues = destOffset;
        argumentDesc.Constant.Num32BitValuesToSet = numValuesToSet;
        m_indirectArgs.push_back(argumentDesc);
    }

    void DirectX12IndirectSignature::AddIndirectShaderResourceViewArg(const uint32_t rootParameterIndex)
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
        argumentDesc.ShaderResourceView.RootParameterIndex = rootParameterIndex;
        m_indirectArgs.push_back(argumentDesc);
    }

    void DirectX12IndirectSignature::AddIndirectDrawArg()
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
        m_indirectArgs.push_back(argumentDesc);
    }
}
