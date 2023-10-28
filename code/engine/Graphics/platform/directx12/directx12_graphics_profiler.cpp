#include <d3d12.h>
#include <pix_win.h>
#include "directx12_graphics_profiler.h"
#include "directx12_graphics_context.h"
#include <algorithm>

namespace KNR
{
	GraphicsProfiler* GraphicsProfiler::Create()
	{
		return new DirectX12GraphicsProfiler();
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
	DirectX12GraphicsProfiler::DirectX12GraphicsProfiler()
	{
		D3D12_QUERY_HEAP_DESC heapDesc = {};
		heapDesc.Count = m_maxProfiles * 2;
		heapDesc.NodeMask = 0;
		heapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		DirectX12Context.GetDevice()->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(&m_queryHeap));

		CreateReadbackBuffer();

		//Set up the profile data with the maximum number of profiles
		m_profiles.resize(m_maxProfiles);
	}

	DirectX12GraphicsProfiler::~DirectX12GraphicsProfiler()
	{
		m_queryHeap->Release(); 
		m_queryHeap = 0;

		m_readbackBuffer->Release(); 
		m_readbackBuffer = 0;
	}

	uint64_t DirectX12GraphicsProfiler::StartProfiler(DirectX12CommandBuffer* commandList, const char* name)
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

	void DirectX12GraphicsProfiler::EndProfiler(DirectX12CommandBuffer* commandList, uint64_t idx)
	{

		const uint32_t startQueryIdx = (uint32_t)(idx * 2);
		const uint32_t endQueryIdx = startQueryIdx + 1;
		commandList->Get()->EndQuery(m_queryHeap, D3D12_QUERY_TYPE_TIMESTAMP, endQueryIdx);

		// Resolve the data
		const uint64_t dstOffset = (startQueryIdx * sizeof(uint64_t));
		commandList->Get()->ResolveQueryData(m_queryHeap, D3D12_QUERY_TYPE_TIMESTAMP, startQueryIdx, 2, m_readbackBuffer, dstOffset);
		PIXEndEvent(commandList->Get());
	}

	void DirectX12GraphicsProfiler::DisplayFrameData()
	{
		uint64_t* frameQueryData;
		uint64_t* queryData;
		uint64_t gpuFrequency;
		DirectX12Context.GetCommandQueue()->GetTimestampFrequency(&gpuFrequency);

		void* mapData;
		m_readbackBuffer->Map(0, nullptr, &mapData);

		queryData = reinterpret_cast<uint64_t*>(mapData);
		frameQueryData = queryData;

		bool drawText = false;
		ImVec2 initialSize = ImVec2(1280 * 0.5f, 720 * 0.25f);
		ImGui::SetNextWindowSize(initialSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);

		drawText = ImGui::Begin("Timing");
		

		if (drawText)
		{
			ImGui::Text("GPU Timing");
			ImGui::Separator();
		}

		for (uint64_t profileIdx = 0; profileIdx < m_numOfProfiles; ++profileIdx)
		{
			Update(m_profiles[profileIdx], profileIdx, drawText, gpuFrequency, frameQueryData);
		}


		ImGui::End();


		m_readbackBuffer->Unmap(0, nullptr);

	}

	void DirectX12GraphicsProfiler::Update(ProfileData& profile, uint64_t profileIdx, bool drawText, uint64_t gpuFrequency, uint64_t* frameQueryData)
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
			maxTime = std::max(profile.TimeSamples[i], maxTime);
			avgTime += profile.TimeSamples[i];
			++avgTimeSamples;
		}

		if (avgTimeSamples > 0)
			avgTime /= double(avgTimeSamples);

		if (profile.active && drawText)
			ImGui::Text("%s: %.2fms (%.2fms max)", profile.name, avgTime, maxTime);

		profile.active = false;
	}

	void DirectX12GraphicsProfiler::CreateReadbackBuffer()
	{
		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = 64 * 2 * sizeof(uint64_t);
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Alignment = 0;
		
		D3D12_HEAP_PROPERTIES heapProps =
		{
			D3D12_HEAP_TYPE_READBACK,
			D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			D3D12_MEMORY_POOL_UNKNOWN,
			0,
			0,
		};

		HRESULT hr = DirectX12Context.GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_readbackBuffer));
		if (FAILED(hr))
		{
			//Test
		}
	}
}