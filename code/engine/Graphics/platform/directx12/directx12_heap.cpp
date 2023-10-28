#include "directx12_heap.h"
#include <d3d12.h>

namespace KNR
{
	DirectX12Heap::~DirectX12Heap()
	{

	}

	HRESULT DirectX12Heap::Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool bShaderVisible)
	{
		HRESULT hr;
		//Set up heap descriptor
		HeapDesc.Type = type;
		HeapDesc.NumDescriptors = numDescriptors;
		HeapDesc.Flags = bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		//Create descriptor heap
		hr = device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&pDescriptorHeap));
		if (FAILED(hr))
		{
			assert(0);
		}

		hCPUHeapStart = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		if (bShaderVisible)
		{
			hGPUHeapStart = pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		}
		else
		{
			hGPUHeapStart.ptr = 0;
		}
		HandleIncrementSize = device->GetDescriptorHandleIncrementSize(HeapDesc.Type);
		return hr;
	}

	UINT64 DirectX12Heap::MakeOffsetted(UINT64 ptr, UINT index)
	{
		UINT64 offsetted;
		offsetted = ptr + static_cast<UINT64>(index * HandleIncrementSize);
		return offsetted;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12Heap::handleCPU(UINT index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = MakeOffsetted(hCPUHeapStart.ptr, index);
		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectX12Heap::handleGPU(UINT index)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = MakeOffsetted(hGPUHeapStart.ptr, index);
		return handle;
	}
}