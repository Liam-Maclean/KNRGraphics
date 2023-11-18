#include "directx12_technique.h"
#include "directx12_graphics_context.h"

namespace KNR
{
	Technique* Technique::Create(const PipelineStateDesc& pipelineCreateInfo)
	{
		return new DirectX12Technique(pipelineCreateInfo);
	}

	DirectX12Technique::DirectX12Technique(const PipelineStateDesc& pipelineCreateInfo)
	{
		//Create the pipeline
		m_pipeline = new DirectX12Pipeline(pipelineCreateInfo);
	}

	DirectX12Technique::~DirectX12Technique()
	{

	}

	void DirectX12Technique::Bind(CommandBuffer* commandList) const
	{
		m_pipeline->Bind(commandList);
	}

	void DirectX12Technique::Unbind(CommandBuffer* commandList) const
	{

	}
}