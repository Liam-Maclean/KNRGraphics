#pragma once

namespace KNR
{
	struct PipelineStateDesc;
	class CommandBuffer;
	class Technique
	{
	public:
		virtual ~Technique() = default;

		inline const std::string GetName() { return m_name; }

		virtual void Bind(CommandBuffer* commandList) const = 0;
		virtual void Unbind(CommandBuffer* commandList) const = 0;

		static Technique* Create(const PipelineStateDesc& pipelineCreateInfo);
	protected:
		std::string m_name;
	};
}