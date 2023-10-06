#pragma once
#include "graphics/platform/directx/directx_render_system.h"
namespace QRender
{
	class TAASystem;
	class RenderSystemManager;
	class FXAASystem : public RenderQuadSystem
	{
	public:
		FXAASystem(RenderSystemManager& manager);
		~FXAASystem();

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
		Ref<QRender::TAASystem> m_taaSystem;
		Ref<Texture2D> m_taaResolveTexture;
		Ref<QRender::DirectXCommandBuffer> m_commandList;
	protected:
	};
}