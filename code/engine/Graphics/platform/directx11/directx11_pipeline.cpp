#include "directx11_pipeline.h"
#include "directx11_graphics_context.h"
#include "render_types.h"

namespace KNR
{
	DirectX11Pipeline::DirectX11Pipeline(const PipelineCreateInfo& desc)
	{
		//HRESULT hr;
		m_pipelineCreateInfo = desc;

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

	DirectX11Pipeline::DirectX11Pipeline()
	{

	}

	DirectX11Pipeline::~DirectX11Pipeline()
	{

	}

	void DirectX11Pipeline::Bind(CommandBuffer* commandList)
	{
		
	}

	void DirectX11Pipeline::Unbind(CommandBuffer* commandList)
	{

	}
}
