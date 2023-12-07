#pragma once
#include "pipeline.h"
#include "directx12_pipeline_layout.h"
#include <vector>
namespace KNR
{
	class CommandBuffer;
	class DirectX12Pipeline final : Pipeline
	{
	public:
		DirectX12Pipeline(const PipelineStateDesc& desc);
		DirectX12Pipeline();
		~DirectX12Pipeline() override final;

		inline DirectX12PipelineLayout* GetPipelineLayout() const { return m_pipelineLayout; }
		inline ID3D12PipelineState* GetPipeline() const {return m_pipelineState; }
	private:
		DirectX12PipelineLayout* m_pipelineLayout; //root signature class
		ID3D12PipelineState* m_pipelineState;
		PipelineStateDesc m_pipelineCreateInfo;
	};
}