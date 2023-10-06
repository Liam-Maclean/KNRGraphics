#pragma once
#include "graphics/renderable_types.h"
#include "graphics/platform/directx/directx_render_system.h"

namespace QRender
{
	class DirectXCommandBuffer;
	class Technique;
	class RenderSystemManager;
	class DOFSystem : public RenderSystem
	{
	public:
		struct RenderData
		{
			RenderData() { };

		};

		DOFSystem(RenderSystemManager& manager);
		~DOFSystem();

		virtual void Init() override;
		virtual void Render() override;
	
		virtual void Update() override;
		virtual void SetUpData() override;
		virtual void Dispatch() override;
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void DebugDraw() override;
		virtual void Resize(int width, int height) override;

	private:
		Ref<QRender::Technique> m_technique;
		Ref<QRender::DirectXCommandBuffer> m_commandList;

	protected:
	};
}