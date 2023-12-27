#include "directx12_pipeline.h"
#include "directx12_util.h"
#include "directx12_vertex_declaration.h"
#include "directx12_graphics_context.h"
#include "logger/logger.h"
#include "render_types.h"
#include "d3dx12.h"

namespace KNR
{
	Pipeline* Pipeline::Create(const PipelineStateDesc& desc)
	{
		return new DirectX12Pipeline(desc);
	}

	DirectX12Pipeline::DirectX12Pipeline(const PipelineStateDesc& desc)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());
		HRESULT hr;
		m_pipelineCreateInfo = desc;
		//m_pipelineLayout = &m_pipelineCreateInfo.pipelineLayout;

		//If the shader is a compute shader, we need an entirely different kind of pipeline create info
		if (m_pipelineCreateInfo.computeBytecode.bytecode != nullptr)
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC computeDesc = {};
			computeDesc.CS.pShaderBytecode = m_pipelineCreateInfo.computeBytecode.bytecode;
			computeDesc.CS.BytecodeLength = m_pipelineCreateInfo.computeBytecode.size;

			hr = device->CreateComputePipelineState(&computeDesc, __uuidof(ID3D12PipelineState), (void**)&m_pipelineState);
			if (FAILED(hr))
			{
				KNT_ERROR("FAILED TO CREATE COMPUTE PIPELINE STATE");
				assert(0);
			}
		}
		else
		{	
			//Start creating the pipeline state
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = static_cast<DirectX12VertexDeclaration*>(m_pipelineCreateInfo.vertexDeclaration)->GetVertexInputLayout();
			//psoDesc.pRootSignature = m_pipelineLayout->GetRootSignature();

			//Rasterizer state
			CD3DX12_RASTERIZER_DESC rasterizer(D3D12_DEFAULT);
			rasterizer.CullMode = Util::ConvertCullMode(m_pipelineCreateInfo.rasterizerState.m_CullingMode);
			rasterizer.FillMode = Util::ConvertFillMode(m_pipelineCreateInfo.rasterizerState.m_FillMode);
			rasterizer.DepthBias = m_pipelineCreateInfo.rasterizerState.m_DepthBias;
			rasterizer.DepthBiasClamp = m_pipelineCreateInfo.rasterizerState.m_DepthBiasClamp;
			if (m_pipelineCreateInfo.rasterizerState.m_ConservativeRasterizer)
			{
				rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
			}

			if (m_pipelineCreateInfo.vertexBytecode.size)
			{
				psoDesc.VS = Util::ConvertBytecode(m_pipelineCreateInfo.vertexBytecode);
			}
			if (m_pipelineCreateInfo.pixelBytecode.size)
			{
				psoDesc.PS = Util::ConvertBytecode(m_pipelineCreateInfo.pixelBytecode);
			}
			if (m_pipelineCreateInfo.hullBytecode.size)
			{
				psoDesc.HS = Util::ConvertBytecode(m_pipelineCreateInfo.hullBytecode);
			}
			if (m_pipelineCreateInfo.domainBytecode.size)
			{
				psoDesc.DS = Util::ConvertBytecode(m_pipelineCreateInfo.domainBytecode);
			}

			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			for (int i = 0; i < m_pipelineCreateInfo.renderTargetState.m_ColorTargets.size(); ++i)
			{
				psoDesc.RTVFormats[i] = Util::GetDXGIFormatType(m_pipelineCreateInfo.renderTargetState.m_ColorTargets[i]);
			}

			if (m_pipelineCreateInfo.renderTargetState.m_DepthTarget != TextureFormat::TEXTURE_FORMAT_NONE)
			{
				psoDesc.DSVFormat = Util::GetDXGIFormatType(m_pipelineCreateInfo.renderTargetState.m_DepthTarget);
			}
			psoDesc.NumRenderTargets = static_cast<UINT>(m_pipelineCreateInfo.renderTargetState.m_ColorTargets.size());


			//Depth stencil state
			CD3DX12_DEPTH_STENCIL_DESC depthStencilState(D3D12_DEFAULT);
			if (m_pipelineCreateInfo.depthStencilState.m_DepthEnable)
			{
				depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
				depthStencilState.DepthFunc = Util::ConvertCompareOp(m_pipelineCreateInfo.depthStencilState.m_DepthOp);
			}
			if (m_pipelineCreateInfo.depthStencilState.m_StencilEnable)
			{
				depthStencilState.StencilEnable = m_pipelineCreateInfo.depthStencilState.m_StencilEnable;
				depthStencilState.StencilWriteMask = m_pipelineCreateInfo.depthStencilState.m_StencilWriteMask;
				depthStencilState.StencilReadMask = m_pipelineCreateInfo.depthStencilState.m_StencilReadMask;
			}

			//Blend states
			CD3DX12_BLEND_DESC blendState(D3D12_DEFAULT);
			for (size_t i = 0; i < 8; ++i)
			{
				if (m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_BlendEnable)
				{
					blendState.RenderTarget[i].BlendEnable = m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_BlendEnable;
					blendState.RenderTarget[i].SrcBlend = Util::ConvertBlendMode(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_ColorSrc);
					blendState.RenderTarget[i].DestBlend = Util::ConvertBlendMode(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_ColorDest);
					blendState.RenderTarget[i].BlendOp = Util::ConvertBlendOp(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_ColorOp);
					blendState.RenderTarget[i].SrcBlendAlpha = Util::ConvertBlendMode(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_AlphaSrc);
					blendState.RenderTarget[i].DestBlendAlpha = Util::ConvertBlendMode(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_AlphaDest);
					blendState.RenderTarget[i].BlendOpAlpha = Util::ConvertBlendOp(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_AlphaOp);
				}
				blendState.RenderTarget[i].LogicOpEnable = m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_LogicOpEnable;
				blendState.RenderTarget[i].RenderTargetWriteMask = m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].m_WriteMask;
			}
			blendState.AlphaToCoverageEnable = m_pipelineCreateInfo.blendState.alphaToCoverageEnable;
			blendState.IndependentBlendEnable = m_pipelineCreateInfo.blendState.independentBlendEnable;

			psoDesc.SampleDesc.Count = 1;
			psoDesc.SampleMask = 0xffffffff;
			psoDesc.DepthStencilState = depthStencilState;
			psoDesc.RasterizerState = rasterizer;
			psoDesc.BlendState = blendState;
			hr = device->CreateGraphicsPipelineState(&psoDesc, __uuidof(ID3D12PipelineState), (void**)&m_pipelineState);
			if (FAILED(hr))
			{
				assert(0);
			}
		}
	}

	DirectX12Pipeline::DirectX12Pipeline()
	{

	}

	DirectX12Pipeline::~DirectX12Pipeline()
	{
		m_pipelineState->Release();
		m_pipelineState = 0;
	}
}
