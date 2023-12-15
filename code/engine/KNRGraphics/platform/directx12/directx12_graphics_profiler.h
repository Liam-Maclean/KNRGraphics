#pragma once
#include "graphics_profiler.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <vector>
namespace KNR
{
	struct ProfileData;
	class DirectX12CommandBuffer;
	class DirectX12GraphicsProfiler final
		: public GraphicsProfiler
	{
	public:
		DirectX12GraphicsProfiler();

		~DirectX12GraphicsProfiler();
	
		virtual void DisplayFrameData() override;

		virtual void Update(ProfileData& profile, const uint64_t profileIdx, const bool drawText, const uint64_t gpuFrequency, const uint64_t* frameQueryData);

		virtual UINT64 StartProfiler(CommandBuffer* commandList, const char* name) override;

		virtual void EndProfiler(CommandBuffer* commandList, const uint64_t idx) override;

		virtual void StartPixMarker(CommandBuffer* commandList, const char* marker) override;

		virtual void EndPixMarker(CommandBuffer* commandList) override;

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