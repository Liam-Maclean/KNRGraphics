#pragma once
#include <vector>
#include "opengl_util.h"
namespace KNR
{
	class CommandBuffer;
	class OpenglPipeline : public Pipeline
	{
	public:
		OpenglPipeline(const PipelineCreateInfo& desc);
		OpenglPipeline();
		~OpenglPipeline();

		virtual void Bind(CommandBuffer* commandList);
		virtual void Unbind(CommandBuffer* commandList);
	private:
		PipelineCreateInfo m_pipelineCreateInfo;
	};
}