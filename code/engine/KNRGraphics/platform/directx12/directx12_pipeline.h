#pragma once
#include "pipeline.h"
#include "directx12_pipeline_layout.h"
#include <vector>
namespace KNR
{
	class CommandBuffer;
	class DirectX12Pipeline final : public Pipeline
	{
	public:
		DirectX12Pipeline(const PipelineStateDesc& desc);
		DirectX12Pipeline();
		~DirectX12Pipeline() override final;

		inline ID3D12PipelineState* GetPipeline() const {return m_pipelineState; }
		inline ID3D12RootSignature* GetRootSignature() const { return m_rootSignature; }
	private:
		ID3D12PipelineState* m_pipelineState;
		ID3D12RootSignature* m_rootSignature;
		PipelineStateDesc m_pipelineCreateInfo;
	};
}