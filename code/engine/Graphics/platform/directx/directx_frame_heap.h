#pragma once
#include "directx_heap.h"

namespace KNR
{
	class DirectXCommandBuffer;
	struct DirectXDescriptorHandleBlock;
	enum class BindlessHeapRegion : int
	{
		TEXTURE1D = 0,
		TEXTURE2D = 1,
		TEXTURE3D = 2,
		CUBEMAPS = 3,

		BINDLESSSTART = TEXTURE2D,		//This will eventually be TEXTURE1D
		BINDLESSEND = TEXTURE2D + 1,	//This will eventually be CUBEMAPS

		BINDLESSCOUNT = CUBEMAPS + 1,
	};

	class DirectXFrameHeap
	{
	public:
		DirectXFrameHeap();
		~DirectXFrameHeap();
		void StartFrame(DirectXCommandBuffer* commandList);
		void EndFrame();
		int CopyAllocate(int numDescriptors, DirectXHeap descriptorHeap, int offset = 0); //We still need this for unbound resources where we bind the entire heap
		void CopyAllocate(DirectXDescriptorHandleBlock& descriptorHeapBlock, int offset = 0); //For binding individual resources
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentCPUHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentGPUHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int baseIndex, int offset = 0);
		void CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc, DirectXDescriptorHandleBlock& descriptorHeapBlock);
		void CreateCBV(D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc, DirectXDescriptorHandleBlock& descriptorHeapBlock);
		ID3D12DescriptorHeap* GetDescriptorHeap();

	private:
		const D3D12_DESCRIPTOR_HEAP_TYPE gpuHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		//We will just create 2048 descriptors (not sure if this is good)
		const int descriptorHeapSize = 2048;

		int preIndex;
		int currentHeapIndex;
		int frameStartIndex;
		DirectXHeap gpuHeap;
		DirectXHeap srvCPUHeap;
	};
}