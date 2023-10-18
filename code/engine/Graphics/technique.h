#pragma once

namespace KNR
{
	struct PipelineCreateInfo;
	class CommandBuffer;
	class Technique
	{
	public:
		virtual ~Technique() = default;

		std::string GetName() { return m_name; }

		virtual void Bind(CommandBuffer* commandList) const = 0;
		virtual void Unbind(CommandBuffer* commandList) const = 0;

		static Technique* Create(PipelineCreateInfo pipelineCreateInfo);
	protected:
		std::string m_name;
	};
}