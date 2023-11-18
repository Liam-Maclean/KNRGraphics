#include "directx12_pipeline.h"
#include "directx12_util.h"
#include "directx12_graphics_context.h"
#include "render_types.h"
#include "d3dx12.h"

namespace KNR
{
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
			computeDesc.CS = m_pipelineCreateInfo.computeShader;

			hr = device->CreateComputePipelineState(&computeDesc, __uuidof(ID3D12PipelineState), (void**)&m_pipelineState);
			if (FAILED(hr))
			{
				assert(0);
			}
		}
		else
		{

			//Rasterizer state
			CD3DX12_RASTERIZER_DESC rasterizer(D3D12_DEFAULT);
			rasterizer.CullMode = Util::ConvertCullMode(m_pipelineCreateInfo.rasterizerState.cullingMode);
			rasterizer.FillMode = Util::ConvertFillMode(m_pipelineCreateInfo.rasterizerState.fillMode);
			rasterizer.DepthBias = m_pipelineCreateInfo.rasterizerState.depthBias;
			rasterizer.DepthBiasClamp = m_pipelineCreateInfo.rasterizerState.depthBiasClamp;
			if (m_pipelineCreateInfo.rasterizerState.conservativeRasterizer)
			{
				rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
			}
			

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = m_pipelineCreateInfo.vertexInputLayout;
			psoDesc.pRootSignature = m_pipelineLayout->GetRootSignature();
			psoDesc.VS = m_pipelineCreateInfo.vertShader;
			psoDesc.PS = m_pipelineCreateInfo.pixelShader;

			if (m_pipelineCreateInfo.hullShader.pShaderBytecode)
			{
				psoDesc.HS = m_pipelineCreateInfo.hullShader;
			}
			if (m_pipelineCreateInfo.domainShader.pShaderBytecode)
			{
				psoDesc.DS = m_pipelineCreateInfo.domainShader;
			}
			psoDesc.PrimitiveTopologyType = m_pipelineCreateInfo.topologyType;

			for (int i = 0; i < m_pipelineCreateInfo.rtvFormats.size(); ++i)
			{
				psoDesc.RTVFormats[i] = m_pipelineCreateInfo.rtvFormats[i];
			}

			//Depth stencil state
			CD3DX12_DEPTH_STENCIL_DESC depthStencilState(D3D12_DEFAULT);
			if (m_pipelineCreateInfo.depthStencilState.depthEnable)
			{
				psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
				depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
				depthStencilState.DepthFunc = Util::ConvertDepthComparisonOp(m_pipelineCreateInfo.depthStencilState.depthComparisonOp);		
			}
			if (m_pipelineCreateInfo.depthStencilState.stencilEnable)
			{
				depthStencilState.StencilEnable = m_pipelineCreateInfo.depthStencilState.stencilEnable;
				depthStencilState.StencilWriteMask = m_pipelineCreateInfo.depthStencilState.stencilWriteMask;
				depthStencilState.StencilReadMask = m_pipelineCreateInfo.depthStencilState.stencilReadMask;
			}

			//Blend states
			CD3DX12_BLEND_DESC blendState(D3D12_DEFAULT);
			for (size_t i = 0; i < 8; ++i)
			{
				if (m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].blendEnable)
				{
					blendState.RenderTarget[i].BlendEnable = m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].blendEnable;

					blendState.RenderTarget[i].SrcBlend = Util::ConvertBlendMode(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].blendSrc);
					blendState.RenderTarget[i].DestBlend = Util::ConvertBlendMode(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].blendDest);
					blendState.RenderTarget[i].BlendOp = Util::ConvertBlendOp(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].blendOp);
					blendState.RenderTarget[i].SrcBlendAlpha = Util::ConvertBlendMode(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].blendDest);
					blendState.RenderTarget[i].DestBlendAlpha = Util::ConvertBlendMode(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].blendDest);
					blendState.RenderTarget[i].BlendOpAlpha = Util::ConvertBlendOp(m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].blendAlphaOp);
				}
				
				if (m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].logicOpEnable)
				{
					blendState.RenderTarget[i].LogicOpEnable = m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].logicOpEnable;
				}
				blendState.RenderTarget[i].RenderTargetWriteMask = m_pipelineCreateInfo.blendState.renderTargetBlendStates[i].writeMask;
			}
			blendState.AlphaToCoverageEnable = m_pipelineCreateInfo.blendState.alphaToCoverageEnable;
			blendState.IndependentBlendEnable = m_pipelineCreateInfo.blendState.independentBlendEnable;
			
			DXGI_SAMPLE_DESC sampleState = {};
			sampleState.

			psoDesc.SampleDesc.Count = 1;
			psoDesc.SampleMask = 0xffffffff;
			psoDesc.DepthStencilState = depthStencilState;
			psoDesc.RasterizerState = rasterizer;
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.NumRenderTargets = static_cast<UINT>(m_pipelineCreateInfo.rtvFormats.size());

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

	void DirectX12Pipeline::Bind(CommandBuffer* commandList)
	{
		DirectX12CommandBuffer* directXCommandBuffer = static_cast<DirectX12CommandBuffer*>(commandList);

		//bind compute
		if (m_pipelineCreateInfo.computeBytecode.bytecode != nullptr)
		{
			directXCommandBuffer->Get()->SetPipelineState(m_pipelineState);
			directXCommandBuffer->Get()->SetGraphicsRootSignature(m_pipelineLayout->GetRootSignature());
		}
		else //bind rasterizer pipeline
		{
			directXCommandBuffer->Get()->SetPipelineState(m_pipelineState);
			directXCommandBuffer->Get()->SetGraphicsRootSignature(m_pipelineLayout->GetRootSignature());
			directXCommandBuffer->Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // HACK LIAM - FIX We probably shouldn't be doing this here
		}
	}

	void DirectX12Pipeline::Unbind(CommandBuffer* commandList)
	{

	}
}
