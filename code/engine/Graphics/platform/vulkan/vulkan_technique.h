#pragma once
#include "technique.h"

namespace KNR
{
	class VulkanPipelinelayout;
	class VulkanPipeline;
	class VulkanTechnique
		: public Technique
	{
	public:
		VulkanTechnique(PipelineCreateInfo pipelineCreateInfo);
		virtual ~VulkanTechnique();

		virtual void Bind(CommandBuffer* commandList) const override;
		virtual void Unbind(CommandBuffer* commandList) const override;
		//VulkanPipelinelayout GetVulkanPipelineLayout() { return m_pipelineLayout; }

	private:
		VulkanPipeline* m_pipeline;
		//VulkanPipelinelayout m_pipelineLayout;
	};
}