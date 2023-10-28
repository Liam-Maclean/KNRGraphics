#pragma once
#include "technique.h"

namespace KNR
{
	class OpenglTechnique
		: public Technique
	{
	public:
		OpenglTechnique(PipelineCreateInfo pipelineCreateInfo);
		virtual ~OpenglTechnique();

		virtual void Bind(CommandBuffer* commandList) const override;
		virtual void Unbind(CommandBuffer* commandList) const override;
	private:
	};
}