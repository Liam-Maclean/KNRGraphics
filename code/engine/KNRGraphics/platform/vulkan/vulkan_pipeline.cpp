#include "vulkan_pipeline.h"
#include "vulkan_graphics_context.h"
#include "render_types.h"

namespace KNR
{
	VulkanPipeline::VulkanPipeline(const PipelineCreateInfo& desc)
	{
		//HRESULT hr;
		m_pipelineCreateInfo = desc;
		m_pipelineLayout = m_pipelineCreateInfo.pipelineLayout;

		//If the shader is a compute shader, we need an entirely different kind of pipeline create info
		if (m_pipelineCreateInfo.computeShader != nullptr)
		{

		}
		else //Rasterizer pipeline
		{
			if (m_pipelineCreateInfo.wireframeModeEnabled)
			{
			}
			else
			{

			}
			if (m_pipelineCreateInfo.cullMode == CullingMode::frontface)
			{

			}
			else if (m_pipelineCreateInfo.cullMode == CullingMode::frontface)
			{

			}
			else
			{

			}

			if (m_pipelineCreateInfo.hullShader)
			{

			}

			if (m_pipelineCreateInfo.domainShader)
			{

			}

			for (int i = 0; i < m_pipelineCreateInfo.rtvFormats.size(); ++i)
			{

			}

			if (m_pipelineCreateInfo.depthTestEnabled)
			{
			
			}
		}
	}

	VulkanPipeline::VulkanPipeline()
	{

	}

	VulkanPipeline::~VulkanPipeline()
	{

	}

	void VulkanPipeline::Bind(CommandBuffer* commandList)
	{
		
	}

	void VulkanPipeline::Unbind(CommandBuffer* commandList)
	{

	}
}
