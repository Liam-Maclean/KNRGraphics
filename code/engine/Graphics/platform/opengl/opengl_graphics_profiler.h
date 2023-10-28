#pragma once
#include "graphics_profiler.h"
#include <vector>
namespace KNR
{
	struct ProfileData;
	class CommandBuffer;
	class OpenglGraphicsProfiler
		: public GraphicsProfiler
	{
	public:
		OpenglGraphicsProfiler();
		~OpenglGraphicsProfiler();
		virtual uint64_t StartProfiler(CommandBuffer* commandList, const char* name) override;
		virtual void EndProfiler(CommandBuffer* commandList, uint64_t idx)override;
		virtual void DisplayFrameData() override;
		virtual void Update(ProfileData& profile, uint64_t profileIdx, bool drawText, uint64_t gpuFrequency, uint64_t* frameQueryData);

	private:
		void CreateReadbackBuffer();

		std::vector<ProfileData> m_profiles;
		uint64_t m_numOfProfiles = 0;
		uint64_t m_maxProfiles = 64;
		uint32_t m_frameCount = 0;
	};
}