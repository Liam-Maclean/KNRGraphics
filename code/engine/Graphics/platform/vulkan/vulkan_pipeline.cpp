#include "vulkan_pipeline.h"
#include "vulkan_graphics_context.h"
#include "render_types.h"

namespace KNR
{
	VulkanPipeline::VulkanPipeline(const PipelineCreateInfo& desc)
	{
		HRESULT hr;
		m_pipelineCreateInfo = desc;
		m_pipelineLayout = &m_pipelineCreateInfo.pipelineLayout;

		//If the shader is a compute shader, we need an entirely different kind of pipeline create info
		if (m_pipelineCreateInfo.computeShader.pShaderBytecode != nullptr)
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

			if (m_pipelineCreateInfo.hullShader.pShaderBytecode)
			{

			}

			if (m_pipelineCreateInfo.domainShader.pShaderBytecode)
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
		//bind compute
		if (m_pipelineCreateInfo.computeShader.pShaderBytecode != nullptr)
		{
			commandList->Get()->SetPipelineState(m_pipelineState);
			commandList->Get()->SetGraphicsRootSignature(m_pipelineLayout->GetRootSignature());

		}
		else //bind rasterizer pipeline
		{
			commandList->Get()->SetPipelineState(m_pipelineState);
			commandList->Get()->SetGraphicsRootSignature(m_pipelineLayout->GetRootSignature());
			commandList->Get()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // HACK LIAM - FIX We probably shouldn't be doing this here
		}
	}

	void VulkanPipeline::Unbind(CommandBuffer* commandList)
	{

	}
}
