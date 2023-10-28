#pragma once
#define DirectX12Context (*KNR::CDirectX12Context::Get())
#include "window.h"
#include "graphics_context.h"
#include "directx12_heap.h"
#include "directx12_frame_heap.h"
#include "directx12_fence.h"
#include "directx12_command_buffer.h"
#include "directx12_swapchain.h"
#include <d3d12.h>
#include <dxgi1_5.h>

#if defined(DIRECTX)
using RENDERERID = uint64_t;
#else
using RENDERERID = uint32_t;
#endif

namespace KNR
{
    struct DirectX12DescriptorHandleBlock {
        DirectX12DescriptorHandleBlock() { cpuHandle.ptr = 0; gpuHandle.ptr = 0; };
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    };

    class CDirectX12Context :
        public CGraphicsContext
    {
    public:
        static CDirectX12Context* Get()
        {
            static auto renderContext = new CDirectX12Context();
            return renderContext;
        }

        CDirectX12Context();
        CDirectX12Context(WindowDesc* windowHandle);
        ~CDirectX12Context();

        void Init(WindowDesc* windowHandle);
        virtual void Init() override;
        virtual void SwapBuffers() override;

        inline ID3D12Device* GetDevice() { return m_device; }
        inline ID3D12GraphicsCommandList* GetCommandList() { return m_commandList; }
        inline ID3D12GraphicsCommandList* GetImguiCommandList() { return m_imGuiCommandList; }
        inline IDXGISwapChain3* GetSwapchain() { return m_swapChain->GetSwapchain(); }
        inline ID3D12CommandQueue* GetCommandQueue() { return m_commandQueue; }
        inline ID3D12CommandQueue* GetComputeQueue() { return m_computeQueue; }
        inline ID3D12CommandAllocator* GetAllocator(int bufferIndex) { return m_commandAllocator[bufferIndex]; }
        inline ID3D12CommandAllocator* GetImguiAllocator() { return m_imGuiCommandAllocator; }
        inline DirectX12Heap* GetImGuiHeap() { return &m_imguiHeap; }
        inline DirectX12FrameHeap* GetFrameHeap() { return m_frameHeap; }
        inline DirectX12CommandBuffer* GetCopyCommandBuffer() { return m_copyCommandBuffer; }


        void StartBlit(ID3D12Resource* srcResource, ID3D12Resource* dstResource, int width, int height);
        void EndBlit();

        void StartBlitToSwapchain(ID3D12Resource* srcResource, ID3D12Resource* dstResource, int width, int height);

        DirectX12DescriptorHandleBlock ReserveDescriptorHandle(ID3D12Resource* resource, BindlessHeapRegion region, D3D12_SHADER_RESOURCE_VIEW_DESC srv, RENDERERID& renderId);
    private:
        
        void CreateDevice();
        void CreateQueues();
        void CreateSwapchain();
        void CreateCommandList();
        void CreateReservedHeapAllocations();

        WindowDesc* m_window;
        ID3D12Device* m_device;
        DirectX12Swapchain* m_swapChain;
        DirectX12Heap m_imguiHeap;
        DirectX12FrameHeap* m_frameHeap;

        DirectX12CommandBuffer* m_copyCommandBuffer;

        ID3D12CommandQueue* m_commandQueue;
        ID3D12CommandQueue* m_computeQueue;

        ID3D12CommandAllocator* m_commandAllocator[3];
        ID3D12GraphicsCommandList* m_commandList;

        ID3D12CommandAllocator* m_blitAllocator;
        ID3D12GraphicsCommandList* m_blitCommandList;

        ID3D12CommandAllocator* m_imGuiCommandAllocator;
        ID3D12GraphicsCommandList* m_imGuiCommandList;

        DirectX12Fence* m_blitFence;

        unsigned int m_videoCardMemory;
        unsigned int m_systemMemory;
        char m_videoCardDescription[128];

        uint32_t m_heapReservationMaxSizes[(int)BindlessHeapRegion::BINDLESSCOUNT];
        uint32_t m_heapReservationUsed[(int)BindlessHeapRegion::BINDLESSCOUNT];
        DirectX12Heap m_heapReservationSlots[(int)BindlessHeapRegion::BINDLESSCOUNT];
    };
}

