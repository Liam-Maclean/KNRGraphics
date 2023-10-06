#pragma once
#define DirectXContext (*QRender::CDirectXContext::Get())
#include "graphics/graphics_context.h"
#include "graphics/platform/directx/directx_heap.h"
#include "graphics/platform/directx/directx_frame_heap.h"
#include "graphics/platform/directx/directx_fence.h"
#include "graphics/platform/directx/directx_command_buffer.h"
#include "platform/window_interface.h"
#include "directx_swapchain.h"
#include <d3d12.h>
#include <dxgi1_5.h>

#if defined(DIRECTX)
using RENDERERID = uint64_t;
#else
using RENDERERID = uint32_t;
#endif

namespace QRender
{
    struct DirectXDescriptorHandleBlock {
        DirectXDescriptorHandleBlock() { cpuHandle.ptr = 0; gpuHandle.ptr = 0; };
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    };

    class CDirectXContext :
        public CGraphicsContext
    {
    public:
        static CDirectXContext* Get()
        {
            static auto renderContext = new CDirectXContext();
            return renderContext;
        }

        CDirectXContext();
        CDirectXContext(IWindow* windowHandle);
        ~CDirectXContext();

        void Init(IWindow* windowHandle);
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
        inline DirectXHeap* GetImGuiHeap() { return &m_imguiHeap; }
        inline DirectXFrameHeap* GetFrameHeap() { return m_frameHeap; }
        inline DirectXCommandBuffer* GetCopyCommandBuffer() { return m_copyCommandBuffer; }


        void StartBlit(ID3D12Resource* srcResource, ID3D12Resource* dstResource, int width, int height);
        void EndBlit();

        void StartBlitToSwapchain(ID3D12Resource* srcResource, ID3D12Resource* dstResource, int width, int height);

        DirectXDescriptorHandleBlock ReserveDescriptorHandle(ID3D12Resource* resource, BindlessHeapRegion region, D3D12_SHADER_RESOURCE_VIEW_DESC srv, RENDERERID& renderId);
    private:
        
        void CreateDevice();
        void CreateQueues();
        void CreateSwapchain();
        void CreateCommandList();
        void CreateReservedHeapAllocations();

        IWindow* m_window;
        ID3D12Device* m_device;
        DirectXSwapchain* m_swapChain;
        DirectXHeap m_imguiHeap;
        DirectXFrameHeap* m_frameHeap;

        DirectXCommandBuffer* m_copyCommandBuffer;

        ID3D12CommandQueue* m_commandQueue;
        ID3D12CommandQueue* m_computeQueue;

        ID3D12CommandAllocator* m_commandAllocator[3];
        ID3D12GraphicsCommandList* m_commandList;

        ID3D12CommandAllocator* m_blitAllocator;
        ID3D12GraphicsCommandList* m_blitCommandList;

        ID3D12CommandAllocator* m_imGuiCommandAllocator;
        ID3D12GraphicsCommandList* m_imGuiCommandList;

        DirectXFence* m_blitFence;

        unsigned int m_videoCardMemory;
        unsigned int m_systemMemory;
        char m_videoCardDescription[128];

        uint32_t m_heapReservationMaxSizes[(int)BindlessHeapRegion::BINDLESSCOUNT];
        uint32_t m_heapReservationUsed[(int)BindlessHeapRegion::BINDLESSCOUNT];
        DirectXHeap m_heapReservationSlots[(int)BindlessHeapRegion::BINDLESSCOUNT];
    };
}

