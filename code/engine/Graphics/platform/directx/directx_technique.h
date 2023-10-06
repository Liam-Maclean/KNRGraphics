#pragma once
#include "graphics/technique.h"

namespace QRender
{
	class DirectXTechnique
		: public Technique
	{
	public:
		DirectXTechnique(PipelineCreateInfo pipelineCreateInfo);
		virtual ~DirectXTechnique();

		virtual void Bind(Ref<DirectXCommandBuffer> commandList) const override;
		virtual void Unbind(Ref<DirectXCommandBuffer> commandList) const override;
		DirectXPipelineLayout* GetDirectXPipelineLayout() { return m_pipeline->GetPipelineLayout(); }
		ID3D12RootSignature* GetRootSignature() { return m_pipeline->GetPipelineLayout()->GetRootSignature(); }
	private:
		DirectXPipeline* m_pipeline;
		DirectXPipelineLayout* m_pipelineLayout;
	private:

	};
}