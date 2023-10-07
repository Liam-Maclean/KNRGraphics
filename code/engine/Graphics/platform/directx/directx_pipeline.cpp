#include "directx_pipeline.h"
#include "directx_graphics_context.h"
#include "render_types.h"
#include "d3dx12.h"

namespace KNR
{
	DirectXPipeline::DirectXPipeline(const PipelineCreateInfo& desc)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectXContext.GetDevice());
		HRESULT hr;
		m_pipelineCreateInfo = desc;
		m_pipelineLayout = &m_pipelineCreateInfo.pipelineLayout;

		//If the shader is a compute shader, we need an entirely different kind of pipeline create info
		if (m_pipelineCreateInfo.computeShader.pShaderBytecode != nullptr)
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC computeDesc = {};
			computeDesc.CS = m_pipelineCreateInfo.computeShader;

			hr = device->CreateComputePipelineState(&computeDesc, __uuidof(ID3D12PipelineState), (void**)&m_pipelineState);
			if (FAILED(hr))
			{
				assert(0);
			}
		}
		else //Rasterizer pipeline
		{
			CD3DX12_RASTERIZER_DESC rasterizer(D3D12_DEFAULT);
			if (m_pipelineCreateInfo.wireframeModeEnabled)
			{
				rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
			}
			else
			{
				rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
			}
			if (m_pipelineCreateInfo.cullMode == CullingMode::frontface)
			{
				rasterizer.CullMode == D3D12_CULL_MODE_FRONT;
			}
			else if (m_pipelineCreateInfo.cullMode == CullingMode::frontface)
			{
				rasterizer.CullMode == D3D12_CULL_MODE_BACK;
			}
			else
			{
				rasterizer.CullMode == D3D12_CULL_MODE_NONE;

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

			if (m_pipelineCreateInfo.depthTestEnabled)
			{
				//psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
				psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
				psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
				psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			}
			psoDesc.SampleDesc.Count = 1;
			psoDesc.SampleMask = 0xffffffff;
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

	DirectXPipeline::DirectXPipeline()
	{

	}

	DirectXPipeline::~DirectXPipeline()
	{
		m_pipelineState->Release();
		m_pipelineState = 0;
	}

	void DirectXPipeline::Bind(DirectXCommandBuffer* commandList)
	{
		//bind compute
		if (m_pipelineCreateInfo.computeShader.pShaderBytecode != nullptr)
		{
			commandList->Get()->SetPipelineState(m_pipelineState);
			commandList->Get()->SetGraphicsRootSignature(m_pipelineLayout->GetRootSignature());

		}
		else //bind rasterizer pipeline
		{
			commandList->Get()->SetPipelineState(m_pipelineState);
			commandList->Get()->SetGraphicsRootSignature(m_pipelineLayout->GetRootSignature());
			commandList->Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // HACK LIAM - FIX We probably shouldn't be doing this here
		}
	}

	void DirectXPipeline::Unbind(DirectXCommandBuffer* commandList)
	{

	}
}
