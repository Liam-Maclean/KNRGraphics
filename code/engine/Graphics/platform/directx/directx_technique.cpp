#include "qlpch.h"
#include "directx_technique.h"
#include "graphics/platform/directx/directx_graphics_context.h"

namespace QRender
{
	Ref<Technique> Technique::Create(PipelineCreateInfo pipelineCreateInfo)
	{
		return MakeRef<DirectXTechnique>(pipelineCreateInfo);
	}

	DirectXTechnique::DirectXTechnique(PipelineCreateInfo pipelineCreateInfo)
	{
		//Create the pipeline
		m_pipeline = new DirectXPipeline(pipelineCreateInfo);
	}

	DirectXTechnique::~DirectXTechnique()
	{

	}

	void DirectXTechnique::Bind(Ref<DirectXCommandBuffer> commandList) const
	{
		m_pipeline->Bind(commandList);
	}

	void DirectXTechnique::Unbind(Ref<DirectXCommandBuffer> commandList) const
	{

	}
}