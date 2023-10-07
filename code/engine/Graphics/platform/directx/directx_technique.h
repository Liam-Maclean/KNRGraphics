#pragma once
#include "technique.h"

namespace KNR
{
	class DirectXTechnique
		: public Technique
	{
	public:
		DirectXTechnique(PipelineCreateInfo pipelineCreateInfo);
		virtual ~DirectXTechnique();

		virtual void Bind(DirectXCommandBuffer* commandList) const override;
		virtual void Unbind(DirectXCommandBuffer* commandList) const override;
		DirectXPipelineLayout* GetDirectXPipelineLayout() { return m_pipeline->GetPipelineLayout(); }
		ID3D12RootSignature* GetRootSignature() { return m_pipeline->GetPipelineLayout()->GetRootSignature(); }
	private:
		DirectXPipeline* m_pipeline;
		DirectXPipelineLayout* m_pipelineLayout;
	};
}