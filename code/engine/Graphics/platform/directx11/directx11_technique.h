#pragma once
#include "technique.h"

namespace KNR
{
	class DirectX11Pipelinelayout;
	class DirectX11Pipeline;
	class DirectX11Technique
		: public Technique
	{
	public:
		DirectX11Technique(PipelineCreateInfo pipelineCreateInfo);
		virtual ~DirectX11Technique();

		virtual void Bind(CommandBuffer* commandList) const override;
		virtual void Unbind(CommandBuffer* commandList) const override;
		//VulkanPipelinelayout GetVulkanPipelineLayout() { return m_pipelineLayout; }

	private:
		DirectX11Pipeline* m_pipeline;
		//VulkanPipelinelayout m_pipelineLayout;
	};
}