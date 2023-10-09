#pragma once
#include "windows.h"
#include <d3d12.h>
namespace KNR
{
	class DirectXHeap
	{
	public:
		DirectXHeap() 
		{ 
			memset(this, 0, sizeof(*this)); 
		}

		~DirectXHeap();

		HRESULT Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool bShaderVisible = false);
		operator ID3D12DescriptorHeap* () { return pDescriptorHeap; }

		UINT64 MakeOffsetted(UINT64 ptr, UINT index);

		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU(UINT index);

		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU(UINT index);

		D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
		D3D12_CPU_DESCRIPTOR_HANDLE hCPUHeapStart;
		D3D12_GPU_DESCRIPTOR_HANDLE hGPUHeapStart;
		UINT CurrentHeapSize = 0;
		UINT HandleIncrementSize;

	public:
		UINT GetCurrentHeapIndex() { return CurrentHeapSize; }
		void IncrementHeapIndex() { CurrentHeapSize++; }
		ID3D12DescriptorHeap* pDescriptorHeap;
	};
}
