#pragma once
#include "directx12_heap.h"
#include "render_types.h"

namespace KNR
{
	class DirectX12CommandBuffer;
	struct DirectX12DescriptorHandleBlock;

	class DirectX12FrameHeap final
	{
	public:
		DirectX12FrameHeap();
		~DirectX12FrameHeap();
		void StartFrame(DirectX12CommandBuffer* commandList);
		void EndFrame();
		int CopyAllocate(const int numDescriptors, DirectX12Heap descriptorHeap, const int offset = 0); //We still need this for unbound resources where we bind the entire heap
		void CopyAllocate(DirectX12DescriptorHandleBlock& descriptorHeapBlock, const int offset = 0); //For binding individual resources
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentCPUHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentGPUHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(const int baseIndex, const int offset = 0);
		void CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc, DirectX12DescriptorHandleBlock& descriptorHeapBlock);
		void CreateCBV(D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc, DirectX12DescriptorHandleBlock& descriptorHeapBlock);
		ID3D12DescriptorHeap* GetDescriptorHeap();

	private:
		const D3D12_DESCRIPTOR_HEAP_TYPE gpuHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		//We will just create 2048 descriptors (not sure if this is good)
		const int descriptorHeapSize = 2048;

		int preIndex;
		int currentHeapIndex;
		int frameStartIndex;
		DirectX12Heap gpuHeap;
		DirectX12Heap srvCPUHeap;
	};
}