#include <pix_win.h>
#include "Vulkan_graphics_profiler.h"
#include "Vulkan_graphics_context.h"
#include <algorithm>

namespace KNR
{
	GraphicsProfiler* GraphicsProfiler::Create()
	{
		return new VulkanGraphicsProfiler();
	}

	struct ProfileData
	{
		const char* name = nullptr;

		bool queryStarted = false;
		bool queryEnded = false;
		bool active = false;

		static const uint64_t FilterSize = 64;
		double TimeSamples[FilterSize] = { };
		uint64_t CurrSample = 0;
	};

	VulkanGraphicsProfiler::VulkanGraphicsProfiler()
	{
		CreateReadbackBuffer();

		//Set up the profile data with the maximum number of profiles
		m_profiles.resize(m_maxProfiles);
	}

	VulkanGraphicsProfiler::~VulkanGraphicsProfiler()
	{

	}

	uint64_t VulkanGraphicsProfiler::StartProfiler(CommandBuffer* commandList, const char* name)
	{
		PIXBeginEvent(commandList->Get(), 0, name);
		uint64_t profileIdx = -1;
		//Loop through the profile data we have set up
		for (int i = 0; i < m_profiles.size(); ++i)
		{
			//If the profile already exists
			if (m_profiles[i].name == name)
			{
				//This is the profile idx we're looking for
				profileIdx = i;
			}
		}

		//if the profile doesn't already exist
		if (profileIdx == -1)
		{
			//Assign the profile index and increment
			profileIdx = m_numOfProfiles++;
			m_profiles[profileIdx].name = name;
		}

		ProfileData& profileData = m_profiles[profileIdx];
		profileData.active = true;

		//Profile idx start and end come in pairs, this is why we multiply by 2
		const int startQueryIdx = int(profileIdx * 2);
		commandList->Get()->EndQuery(m_queryHeap, D3D12_QUERY_TYPE_TIMESTAMP, startQueryIdx);

		profileData.queryStarted = true;

		return profileIdx;
	}

	void VulkanGraphicsProfiler::EndProfiler(CommandBuffer* commandList, uint64_t idx)
	{

		const uint32_t startQueryIdx = (uint32_t)(idx * 2);
		const uint32_t endQueryIdx = startQueryIdx + 1;
		commandList->Get()->EndQuery(m_queryHeap, D3D12_QUERY_TYPE_TIMESTAMP, endQueryIdx);

		// Resolve the data
		const uint64_t dstOffset = (startQueryIdx * sizeof(uint64_t));
		commandList->Get()->ResolveQueryData(m_queryHeap, D3D12_QUERY_TYPE_TIMESTAMP, startQueryIdx, 2, m_readbackBuffer, dstOffset);

		PIXEndEvent(commandList->Get());
	}

	void VulkanGraphicsProfiler::DisplayFrameData()
	{
		uint64_t* frameQueryData;
		uint64_t* queryData;
		uint64_t gpuFrequency;

	}

	void VulkanGraphicsProfiler::Update(ProfileData& profile, uint64_t profileIdx, bool drawText, uint64_t gpuFrequency, uint64_t* frameQueryData)
	{
		m_frameCount++;
		profile.queryEnded = false;

		double time = 0.0f;
		if (frameQueryData)
		{
			//Assert_(frameQueryData != nullptr);

			// Get the query data
			uint64_t startTime = frameQueryData[profileIdx * 2 + 0];
			uint64_t endTime = frameQueryData[profileIdx * 2 + 1];

			if (endTime > startTime)
			{
				uint64_t delta = endTime - startTime;
				double frequency = double(gpuFrequency);
				time = (delta / (double)frequency) * 1000.0;
			}
		}

		profile.TimeSamples[profile.CurrSample] = time;
		profile.CurrSample = (profile.CurrSample + 1) % ProfileData::FilterSize;

		double maxTime = 0.0;
		double avgTime = 0.0;
		uint64_t avgTimeSamples = 0;
		for (UINT i = 0; i < ProfileData::FilterSize; ++i)
		{
			if (profile.TimeSamples[i] <= 0.0)
				continue;
			//maxTime = std::max(profile.TimeSamples[i], maxTime);
			avgTime += profile.TimeSamples[i];
			++avgTimeSamples;
		}

		if (avgTimeSamples > 0)
			avgTime /= double(avgTimeSamples);

		//if (profile.active && drawText)
			//ImGui::Text("%s: %.2fms (%.2fms max)", profile.name, avgTime, maxTime);

		profile.active = false;
	}

	void VulkanGraphicsProfiler::CreateReadbackBuffer()
	{

	}
}