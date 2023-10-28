#pragma once
#include "graphics_profiler.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <vector>
namespace KNR
{
	struct ProfileData;
	class DirectX12CommandBuffer;
	class DirectX12GraphicsProfiler
		: public GraphicsProfiler
	{
	public:
		DirectX12GraphicsProfiler();
		~DirectX12GraphicsProfiler();
		virtual uint64_t StartProfiler(DirectX12CommandBuffer* commandList, const char* name) override;
		virtual void EndProfiler(DirectX12CommandBuffer* commandList, uint64_t idx)override;
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