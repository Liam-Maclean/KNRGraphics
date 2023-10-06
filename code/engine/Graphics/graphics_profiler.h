#pragma once
#include "core/core.h"
namespace QRender
{
	struct ProfileData;
	class DirectXCommandBuffer;
	class GraphicsProfiler
	{
	public:
		GraphicsProfiler() {};
		~GraphicsProfiler() {};
		virtual UINT64 StartProfiler(Ref<DirectXCommandBuffer> commandList, const char* name) = 0;
		virtual void EndProfiler(Ref<DirectXCommandBuffer> commandList, UINT64 idx) = 0;
		virtual void DisplayFrameData() = 0;
		virtual void Update(ProfileData& profile, UINT64 profileIdx, bool drawText, UINT64 gpuFrequency, UINT64* frameQueryData) = 0;
		static Ref<GraphicsProfiler> Create();
	};
}