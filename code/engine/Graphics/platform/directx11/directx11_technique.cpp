#include "directx11_technique.h"
#include "directx11_pipeline.h"
#include "directx11_graphics_context.h"

namespace KNR
{
	Technique* Technique::Create(PipelineCreateInfo pipelineCreateInfo)
	{
		return new DirectX11Technique(pipelineCreateInfo);
	}

	DirectX11Technique::DirectX11Technique(PipelineCreateInfo pipelineCreateInfo)
	{
		//Create the pipeline
		m_pipeline = new DirectX11Pipeline(pipelineCreateInfo);
	}

	DirectX11Technique::~DirectX11Technique()
	{

	}

	void DirectX11Technique::Bind(CommandBuffer* commandList) const
	{
		
	}

	void DirectX11Technique::Unbind(CommandBuffer* commandList) const
	{

	}
}