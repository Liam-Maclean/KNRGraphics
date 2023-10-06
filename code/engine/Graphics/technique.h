#pragma once
#include "core/core.h"
#include "logging/log.h"
#include "maths/maths.h"
#include "graphics/platform/directx/directx_pipeline.h"


namespace QRender
{
	class DirectXCommandBuffer;
	class Technique
	{
	public:
		virtual ~Technique() = default;

		std::string GetName() { return m_name; }

		virtual void Bind(Ref<DirectXCommandBuffer> commandList) const = 0;
		virtual void Unbind(Ref<DirectXCommandBuffer> commandList) const = 0;

		static Ref<Technique> Create(PipelineCreateInfo pipelineCreateInfo);
	protected:
		std::string m_name;
	};
}