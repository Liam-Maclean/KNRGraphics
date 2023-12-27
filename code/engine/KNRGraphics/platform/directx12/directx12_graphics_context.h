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
        DirectX12DescriptorHandleBlock() { cpuHandle.ptr = 0; gpuHandle.ptr = 0; bindlessIndexID = 0; };
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
        uint32_t bindlessIndexID;
    };

    class DirectX12Buffer;
    class CDirectX12Context final :
        public CGraphicsContext
    {
    public:
        static CDirectX12Context* Get()
        {
            static auto renderContext = new CDirectX12Context();
            return renderContext;
        }

        CDirectX12Context();

        CDirectX12Context(const WindowDesc& windowHandle);

        ~CDirectX12Context();

        void Init(const WindowDesc& windowHandle);

        void Init() override;

        void SwapBuffers() override;

        DirectX12DescriptorHandleBlock ReserveDescriptorHandle(ReservedHeapRegion region);
    
        void CreateSRV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_SHADER_RESOURCE_VIEW_DESC srv);

        void CreateCBV(const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_CONSTANT_BUFFER_VIEW_DESC cbv);

        void CreateUAV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC uav);
        
        void CreateRTV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_RENDER_TARGET_VIEW_DESC rtv);

        void CreateBackbufferRTV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle);

        void CreateDSV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_DEPTH_STENCIL_VIEW_DESC dsv);

        //Getters
        inline ID3D12Device* GetDevice() { return m_device; }

        inline ID3D12GraphicsCommandList* GetCommandList() { return m_commandList; }

        inline IDXGISwapChain3* GetSwapchain() { return m_swapChain->GetSwapchain(); }

        inline ID3D12CommandQueue* GetCommandQueue() { return m_commandQueue; }

        inline ID3D12CommandQueue* GetComputeQueue() { return m_computeQueue; }

        inline ID3D12CommandAllocator* GetAllocator(int bufferIndex) { return m_commandAllocator[bufferIndex]; }

        inline DirectX12CommandBuffer* GetCopyCommandBuffer() { return m_copyCommandBuffer; }

        inline DirectX12FrameHeap* GetFrameHeap() { return m_frameHeap; }
        
        inline DirectX12Heap* GetReservedHeap(ReservedHeapRegion region) { return &m_bindlessReservationHeaps[(int)region]; }

        inline ID3D12RootSignature* GetKNRRootSignature() const { return m_coreRootSignature; }

    private:
        
        void CreateDevice();

        void CreateQueues();

        void CreateCommandList();

        void CreateBindlessHeapReservations();

        ID3D12Device* m_device;
        ID3D12CommandQueue* m_commandQueue;
        ID3D12CommandQueue* m_computeQueue;
        ID3D12CommandAllocator* m_commandAllocator[3];
        ID3D12GraphicsCommandList* m_commandList;

        ID3D12RootSignature* m_coreRootSignature;

        DirectX12CommandBuffer* m_copyCommandBuffer;
        DirectX12Swapchain* m_swapChain;
        DirectX12FrameHeap* m_frameHeap;
        DirectX12Buffer* m_immediateVertexBuffer;
       
        WindowDesc m_window;
        unsigned int m_videoCardMemory;
        unsigned int m_systemMemory;
        char m_videoCardDescription[128];

        uint32_t m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::HeapRegionCount];
        uint32_t m_bindlessReservationHeapUsed[(int)ReservedHeapRegion::HeapRegionCount];
        DirectX12Heap m_bindlessReservationHeaps[(int)ReservedHeapRegion::HeapRegionCount];
    };
}

