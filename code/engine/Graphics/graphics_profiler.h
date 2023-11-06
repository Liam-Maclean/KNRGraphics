#pragma once
#include <basetsd.h>

namespace KNR
{
	struct ProfileData;
	class CommandBuffer;
	class GraphicsProfiler
	{
	public:
		GraphicsProfiler() {};
		~GraphicsProfiler() {};
		virtual UINT64 StartProfiler(CommandBuffer* commandList, const char* name) = 0;
		virtual void EndProfiler(CommandBuffer* commandList, const UINT64 idx) = 0;
		virtual void DisplayFrameData() = 0;
		virtual void Update(ProfileData& profile, const UINT64 profileIdx, const bool drawText, const UINT64 gpuFrequency, const UINT64* frameQueryData) = 0;
		static GraphicsProfiler* Create();
	};
}