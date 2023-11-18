#pragma once
#include "technique.h"
#include "directx12_pipeline_layout.h"
#include "directx12_pipeline.h"
namespace KNR
{
	class DirectX12Technique
		: public Technique
	{
	public:
		DirectX12Technique(const PipelineStateDesc& pipelineCreateInfo);
		virtual ~DirectX12Technique();

		virtual void Bind(CommandBuffer* commandList) const override;
		virtual void Unbind(CommandBuffer* commandList) const override;
		DirectX12PipelineLayout* GetDirectX12PipelineLayout() { return m_pipeline->GetPipelineLayout(); }
		ID3D12RootSignature* GetRootSignature() { return m_pipeline->GetPipelineLayout()->GetRootSignature(); }
	private:
		DirectX12Pipeline* m_pipeline;
		DirectX12PipelineLayout* m_pipelineLayout;
	};
}