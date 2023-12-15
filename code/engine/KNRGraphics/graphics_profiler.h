#pragma once
#include <basetsd.h>

namespace KNR
{
	struct ProfileData;
	class CommandBuffer;
	class GraphicsProfiler
	{
	public:
		virtual UINT64 StartProfiler(CommandBuffer* commandList, const char* name) = 0;
		virtual void EndProfiler(CommandBuffer* commandList, const UINT64 idx) = 0;
		virtual void StartPixMarker(CommandBuffer* commandList, const char* marker) = 0;
		virtual void EndPixMarker(CommandBuffer* commandList) = 0;
		virtual void DisplayFrameData() = 0;
		virtual void Update(ProfileData& profile, const UINT64 profileIdx, const bool drawText, const UINT64 gpuFrequency, const UINT64* frameQueryData) = 0;
		static GraphicsProfiler* Create();
	};
}