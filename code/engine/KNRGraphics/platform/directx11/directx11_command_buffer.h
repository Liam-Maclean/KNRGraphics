#pragma once
#include "command_buffer.h"
#include "directx11_fence.h"
#include "delegate/callback.h"

namespace KNR
{
	class DirectX11CommandBuffer : public CommandBuffer
	{
	public:
		DirectX11CommandBuffer(CommandBufferType type);
		~DirectX11CommandBuffer();

	private:
		void AddToCommandCallbackList(Delegate<>::Func_type&& func);

		CommandBufferType GetType() { return m_type; }
		bool GetWorkToBeSubmitted() { return m_workToBeSubmitted; }

	private:
		DirectX11Fence* m_fence;
		CommandBufferType m_type;
		Delegate<> m_commandBufferCallbackList;

		bool m_workToBeSubmitted;
		
	};


}