#pragma once
#include "technique.h"

namespace KNR
{
	class DirectX12Technique
		: public Technique
	{
	public:
		DirectX12Technique(PipelineCreateInfo pipelineCreateInfo);
		virtual ~DirectX12Technique();

		virtual void Bind(DirectX12CommandBuffer* commandList) const override;
		virtual void Unbind(DirectX12CommandBuffer* commandList) const override;
		DirectX12PipelineLayout* GetDirectX12PipelineLayout() { return m_pipeline->GetPipelineLayout(); }
		ID3D12RootSignature* GetRootSignature() { return m_pipeline->GetPipelineLayout()->GetRootSignature(); }
	private:
		DirectX12Pipeline* m_pipeline;
		DirectX12PipelineLayout* m_pipelineLayout;
	};
}