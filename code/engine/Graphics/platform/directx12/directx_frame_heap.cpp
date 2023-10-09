#include "directx_frame_heap.h"
#include "directx_graphics_context.h"

namespace KNR
{
	DirectXFrameHeap::DirectXFrameHeap()
	{
		gpuHeap.Create(DirectXContext.GetDevice(), gpuHeapType, descriptorHeapSize, TRUE);
		srvCPUHeap.Create(DirectXContext.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorHeapSize);
		currentHeapIndex = 0;
		preIndex = 0;
		frameStartIndex = 0;
	}

	DirectXFrameHeap::~DirectXFrameHeap()
	{

	}

	void DirectXFrameHeap::StartFrame(DirectXCommandBuffer* commandList)
	{
		frameStartIndex = currentHeapIndex;

		//Set the descriptor heap for the rendering to begin
		ID3D12DescriptorHeap* frameHeap[] = { gpuHeap };
		commandList->Get()->SetDescriptorHeaps(_countof(frameHeap), frameHeap);
	}

	void DirectXFrameHeap::EndFrame()
	{

	}

	int DirectXFrameHeap::CopyAllocate(int numDescriptors, DirectXHeap descriptorHeap, int offset)
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
		DirectXContext.GetDevice()->CopyDescriptorsSimple(numDescriptors, gpuHeap.handleCPU(currentHeapIndex), descriptorHeap.handleCPU(offset), gpuHeapType);
		currentHeapIndex += numDescriptors;
		return preIndex;
	}

	void DirectXFrameHeap::CopyAllocate(DirectXDescriptorHandleBlock& descriptorHeapBlock, int offset)
	{
		auto futureIndex = currentHeapIndex + 1;
		if (futureIndex >= descriptorHeapSize)
		{
			currentHeapIndex = 0;
			frameStartIndex = 0;
		}

		preIndex = currentHeapIndex;

		//Add on the new descriptor heap onto the end of the current index
		DirectXContext.GetDevice()->CopyDescriptorsSimple(1, gpuHeap.handleCPU(currentHeapIndex), descriptorHeapBlock.cpuHandle, gpuHeapType);
		descriptorHeapBlock.gpuHandle = gpuHeap.handleGPU(currentHeapIndex); //<---Is this needed?
		descriptorHeapBlock.cpuHandle = gpuHeap.handleCPU(currentHeapIndex);
		currentHeapIndex += 1;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectXFrameHeap::GetCurrentCPUHandle()
	{
		return gpuHeap.handleCPU(currentHeapIndex);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectXFrameHeap::GetCurrentGPUHandle()
	{
		return gpuHeap.handleGPU(currentHeapIndex);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectXFrameHeap::GetGPUHandle(int baseIndex, int offset)
	{
		return gpuHeap.handleGPU(baseIndex + offset);
	}

	void DirectXFrameHeap::CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc, DirectXDescriptorHandleBlock& descriptorHeapBlock)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectXContext.GetDevice());
		device->CreateShaderResourceView(resource, &srvDesc, srvCPUHeap.handleCPU(srvCPUHeap.GetCurrentHeapIndex()));
		descriptorHeapBlock.cpuHandle = srvCPUHeap.handleCPU(srvCPUHeap.GetCurrentHeapIndex());
		descriptorHeapBlock.gpuHandle = srvCPUHeap.handleGPU(srvCPUHeap.GetCurrentHeapIndex());
		srvCPUHeap.IncrementHeapIndex(); //Increment the heap index
		CopyAllocate(descriptorHeapBlock);
	}

	void DirectXFrameHeap::CreateCBV(D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc, DirectXDescriptorHandleBlock& descriptorHeapBlock)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectXContext.GetDevice());
		device->CreateConstantBufferView(&cbvDesc, srvCPUHeap.handleCPU(srvCPUHeap.GetCurrentHeapIndex()));
		descriptorHeapBlock.cpuHandle = srvCPUHeap.handleCPU(srvCPUHeap.GetCurrentHeapIndex());
		descriptorHeapBlock.gpuHandle = srvCPUHeap.handleGPU(srvCPUHeap.GetCurrentHeapIndex());
		srvCPUHeap.IncrementHeapIndex(); //Increment the heap index
		CopyAllocate(descriptorHeapBlock);
	}


	ID3D12DescriptorHeap* DirectXFrameHeap::GetDescriptorHeap()
	{
		return gpuHeap.pDescriptorHeap;
	}
}
