#include "directx_technique.h"
#include "directx_graphics_context.h"

namespace KNR
{
	Technique* Technique::Create(PipelineCreateInfo pipelineCreateInfo)
	{
		return new DirectXTechnique(pipelineCreateInfo);
	}

	DirectXTechnique::DirectXTechnique(PipelineCreateInfo pipelineCreateInfo)
	{
		//Create the pipeline
		m_pipeline = new DirectXPipeline(pipelineCreateInfo);
	}

	DirectXTechnique::~DirectXTechnique()
	{

	}

	void DirectXTechnique::Bind(DirectXCommandBuffer* commandList) const
	{
		m_pipeline->Bind(commandList);
	}

	void DirectXTechnique::Unbind(DirectXCommandBuffer* commandList) const
	{

	}
}