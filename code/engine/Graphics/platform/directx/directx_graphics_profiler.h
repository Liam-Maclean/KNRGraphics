#pragma once
#include "graphics/graphics_profiler.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <vector>
namespace QRender
{
	struct ProfileData;
	class DirectXCommandBuffer;
	class DirectXGraphicsProfiler
		: public GraphicsProfiler
	{
	public:
		DirectXGraphicsProfiler();
		~DirectXGraphicsProfiler();
		virtual uint64_t StartProfiler(Ref<DirectXCommandBuffer> commandList, const char* name) override;
		virtual void EndProfiler(Ref<DirectXCommandBuffer> commandList, uint64_t idx)override;
		virtual void DisplayFrameData() override;
		virtual void Update(ProfileData& profile, uint64_t profileIdx, bool drawText, uint64_t gpuFrequency, uint64_t* frameQueryData);

	private:
		void CreateReadbackBuffer();

		std::vector<ProfileData> m_profiles;
		ID3D12QueryHeap* m_queryHeap = nullptr;
		ID3D12Resource* m_readbackBuffer = nullptr;
		uint64_t m_numOfProfiles = 0;
		uint64_t m_maxProfiles = 64;
		uint32_t m_frameCount = 0;
	};
}