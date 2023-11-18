#pragma once
#include "command_buffer.h"
#include "delegate/callback.h"
#include "render_types.h"
#include <d3d12.h>
namespace KNR
{
	class DirectX12Fence;
	class DirectX12CommandBuffer final 
		: public CommandBuffer
	{
	public:
		DirectX12CommandBuffer(CommandBufferType type);
		~DirectX12CommandBuffer();

		void Reset();
		void Reset(ID3D12CommandAllocator* customAllocator);
		void Close();
		void Submit(ID3D12CommandQueue* queue);
		void SubmitWorkImmediate();
		void Wait();
		void UpdateCopyResource(ID3D12Resource* dstResource, ID3D12Resource* srcResource, std::vector<D3D12_SUBRESOURCE_DATA> subresources);
		void UpdateCopyResource(ID3D12Resource* dstResource, ID3D12Resource* srcResource, D3D12_SUBRESOURCE_DATA subresources);
		void CopyResource(ID3D12Resource* dstResource, ID3D12Resource* srcResource);
		void AddToCommandCallbackList(Delegate<>::Func_type&& func);

		inline bool GetWorkToBeSubmitted() { return m_workToBeSubmitted; }
		inline ID3D12GraphicsCommandList* Get() { return m_commandList; }; //Liam - We shouldn't really be using this but it's a LOT of work to 
																		   //handle all the cases where we want to replace the old command buffer all
																		   //at once and keep it working, I'll come back to remove direct access to the buffer later
	private:
		ID3D12GraphicsCommandList* m_commandList;
		ID3D12CommandAllocator* m_commandAllocator;
		DirectX12Fence* m_fence;
		bool m_workToBeSubmitted;
		Delegate<> m_commandBufferCallbackList;
	};
}