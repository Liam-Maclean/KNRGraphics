#include "directx12_frame_heap.h"
#include "directx12_graphics_context.h"

namespace KNR
{
	DirectX12FrameHeap::DirectX12FrameHeap()
	{
		gpuHeap.Create(DirectX12Context.GetDevice(), gpuHeapType, descriptorHeapSize, TRUE);
		srvCPUHeap.Create(DirectX12Context.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorHeapSize);
		currentHeapIndex = 0;
		preIndex = 0;
		frameStartIndex = 0;
	}

	DirectX12FrameHeap::~DirectX12FrameHeap()
	{

	}

	void DirectX12FrameHeap::StartFrame(DirectX12CommandBuffer* commandList)
	{
		frameStartIndex = currentHeapIndex;

		//Set the descriptor heap for the rendering to begin
		ID3D12DescriptorHeap* frameHeap[] = { gpuHeap };
		commandList->Get()->SetDescriptorHeaps(_countof(frameHeap), frameHeap);
	}

	void DirectX12FrameHeap::EndFrame()
	{

	}

	int DirectX12FrameHeap::CopyAllocate(const int numDescriptors, DirectX12Heap descriptorHeap, const int offset)
	{
		assert(numDescriptors > 0);
		auto futureIndex = currentHeapIndex + numDescriptors;
		if (futureIndex >= descriptorHeapSize)
		{
			currentHeapIndex = 0;
			frameStartIndex = 0;
		}

		preIndex = currentHeapIndex;

		//Add on the new descriptor heap onto the end of the current index
		DirectX12Context.GetDevice()->CopyDescriptorsSimple(numDescriptors, gpuHeap.handleCPU(currentHeapIndex), descriptorHeap.handleCPU(offset), gpuHeapType);
		currentHeapIndex += numDescriptors;
		return preIndex;
	}

	void DirectX12FrameHeap::CopyAllocate(DirectX12DescriptorHandleBlock& descriptorHeapBlock, const int offset)
	{
		auto futureIndex = currentHeapIndex + 1;
		if (futureIndex >= descriptorHeapSize)
		{
			currentHeapIndex = 0;
			frameStartIndex = 0;
		}

		preIndex = currentHeapIndex;

		//Add on the new descriptor heap onto the end of the current index
		DirectX12Context.GetDevice()->CopyDescriptorsSimple(1, gpuHeap.handleCPU(currentHeapIndex), descriptorHeapBlock.cpuHandle, gpuHeapType);
		descriptorHeapBlock.gpuHandle = gpuHeap.handleGPU(currentHeapIndex); //<---Is this needed?
		descriptorHeapBlock.cpuHandle = gpuHeap.handleCPU(currentHeapIndex);
		currentHeapIndex += 1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12FrameHeap::GetCurrentCPUHandle()
	{
		return gpuHeap.handleCPU(currentHeapIndex);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectX12FrameHeap::GetCurrentGPUHandle()
	{
		return gpuHeap.handleGPU(currentHeapIndex);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectX12FrameHeap::GetGPUHandle(const int baseIndex, const int offset)
	{
		return gpuHeap.handleGPU(baseIndex + offset);
	}

	void DirectX12FrameHeap::CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc, DirectX12DescriptorHandleBlock& descriptorHeapBlock)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());
		device->CreateShaderResourceView(resource, &srvDesc, srvCPUHeap.handleCPU(srvCPUHeap.GetCurrentHeapIndex()));
		descriptorHeapBlock.cpuHandle = srvCPUHeap.handleCPU(srvCPUHeap.GetCurrentHeapIndex());
		descriptorHeapBlock.gpuHandle = srvCPUHeap.handleGPU(srvCPUHeap.GetCurrentHeapIndex());
		srvCPUHeap.IncrementHeapIndex(); //Increment the heap index
		CopyAllocate(descriptorHeapBlock);
	}

	void DirectX12FrameHeap::CreateCBV(D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc, DirectX12DescriptorHandleBlock& descriptorHeapBlock)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());
		device->CreateConstantBufferView(&cbvDesc, srvCPUHeap.handleCPU(srvCPUHeap.GetCurrentHeapIndex()));
		descriptorHeapBlock.cpuHandle = srvCPUHeap.handleCPU(srvCPUHeap.GetCurrentHeapIndex());
		descriptorHeapBlock.gpuHandle = srvCPUHeap.handleGPU(srvCPUHeap.GetCurrentHeapIndex());
		srvCPUHeap.IncrementHeapIndex(); //Increment the heap index
		CopyAllocate(descriptorHeapBlock);
	}


	ID3D12DescriptorHeap* DirectX12FrameHeap::GetDescriptorHeap()
	{
		return gpuHeap.pDescriptorHeap;
	}
}
