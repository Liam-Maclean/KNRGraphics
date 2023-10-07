#pragma once
#include "platform/directx/directx_pipeline.h"


namespace KNR
{
	class DirectXCommandBuffer;
	class Technique
	{
	public:
		virtual ~Technique() = default;

		std::string GetName() { return m_name; }

		virtual void Bind(DirectXCommandBuffer* commandList) const = 0;
		virtual void Unbind(DirectXCommandBuffer* commandList) const = 0;

		static Technique* Create(PipelineCreateInfo pipelineCreateInfo);
	protected:
		std::string m_name;
	};
}