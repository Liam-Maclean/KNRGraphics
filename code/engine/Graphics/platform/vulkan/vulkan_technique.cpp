#include "vulkan_technique.h"
#include "vulkan_pipeline.h"
#include "Vulkan_graphics_context.h"

namespace KNR
{
	Technique* Technique::Create(PipelineCreateInfo pipelineCreateInfo)
	{
		return new VulkanTechnique(pipelineCreateInfo);
	}

	VulkanTechnique::VulkanTechnique(PipelineCreateInfo pipelineCreateInfo)
	{
		//Create the pipeline
		m_pipeline = new VulkanPipeline(pipelineCreateInfo);
	}

	VulkanTechnique::~VulkanTechnique()
	{

	}

	void VulkanTechnique::Bind(DirectXCommandBuffer* commandList) const
	{
		
	}

	void VulkanTechnique::Unbind(DirectXCommandBuffer* commandList) const
	{

	}
}