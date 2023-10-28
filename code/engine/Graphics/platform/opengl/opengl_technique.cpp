#include "opengl_technique.h"
#include "opengl_pipeline.h"
#include "opengl_graphics_context.h"

namespace KNR
{
	Technique* Technique::Create(PipelineCreateInfo pipelineCreateInfo)
	{
		return new OpenglTechnique(pipelineCreateInfo);
	}

	OpenglTechnique::OpenglTechnique(PipelineCreateInfo pipelineCreateInfo)
	{

	}

	OpenglTechnique::~OpenglTechnique()
	{

	}

	void OpenglTechnique::Bind(CommandBuffer* commandList) const
	{
		
	}

	void OpenglTechnique::Unbind(CommandBuffer* commandList) const
	{

	}
}