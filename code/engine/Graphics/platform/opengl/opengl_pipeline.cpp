#include "opengl_pipeline.h"
#include "opengl_graphics_context.h"
#include "render_types.h"

namespace KNR
{
	OpenglPipeline::OpenglPipeline(const PipelineCreateInfo& desc)
	{
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

	OpenglPipeline::OpenglPipeline()
	{

	}

	OpenglPipeline::~OpenglPipeline()
	{

	}

	void OpenglPipeline::Bind(CommandBuffer* commandList)
	{
		
	}

	void OpenglPipeline::Unbind(CommandBuffer* commandList)
	{

	}
}
