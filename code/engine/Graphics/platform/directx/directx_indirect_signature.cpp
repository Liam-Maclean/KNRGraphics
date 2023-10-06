#include <qlpch.h>
#include "graphics/platform/directx/directx_graphics_context.h"
#include "graphics/platform/directx/directx_technique.h"
#include "directx_indirect_signature.h"
namespace QRender
{
	IndirectSignature::IndirectSignature()
	{
      
	}

	IndirectSignature::~IndirectSignature()
	{

	}

    void IndirectSignature::CreateSignature(Ref<Technique> technique, uint32_t byteStride)
    {

     
        ID3D12Device* device = DirectXContext.GetDevice();
        HRESULT hr;
        // Each command consists of a CBV update and a DrawInstanced call.
        D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
        commandSignatureDesc.pArgumentDescs = m_indirectArgs.data();
        commandSignatureDesc.NumArgumentDescs = static_cast<uint32_t>(m_indirectArgs.size());
        commandSignatureDesc.ByteStride = byteStride;
        hr = device->CreateCommandSignature(&commandSignatureDesc, technique ? std::static_pointer_cast<DirectXTechnique>(technique)->GetRootSignature() : nullptr, IID_PPV_ARGS(&m_commandSignature));
        if (FAILED(hr))
        {
            assert(false);
        }
    }

    void IndirectSignature::AddIndirectConstantBufferViewArg(uint32_t rootParameterIndex)
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
        argumentDesc.ConstantBufferView.RootParameterIndex = rootParameterIndex;
        m_indirectArgs.push_back(argumentDesc);
    }

    void IndirectSignature::AddIndirectConstantArg(uint32_t rootParameterIndex, uint32_t destOffset, uint32_t numValuesToSet)
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
        argumentDesc.Constant.RootParameterIndex = rootParameterIndex;
        argumentDesc.Constant.DestOffsetIn32BitValues = destOffset;
        argumentDesc.Constant.Num32BitValuesToSet = numValuesToSet;
        m_indirectArgs.push_back(argumentDesc);
    }

    void IndirectSignature::AddIndirectShaderResourceViewArg(uint32_t rootParameterIndex)
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
        argumentDesc.ShaderResourceView.RootParameterIndex = rootParameterIndex;
        m_indirectArgs.push_back(argumentDesc);
    }

    void IndirectSignature::AddIndirectDrawArg()
    {
        D3D12_INDIRECT_ARGUMENT_DESC argumentDesc = {};
        argumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
        m_indirectArgs.push_back(argumentDesc);
    }


}
