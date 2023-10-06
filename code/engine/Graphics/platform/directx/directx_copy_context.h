#pragma once

#include "graphics/platform/directx/directx_fence.h"
#include "graphics/platform/directx/directx_command_buffer.h"
#include "utilities/callback.h"
#include <d3d12.h>
#include <dxgi1_5.h>
struct GLFWwindow;
namespace QRender
{
    class CDirectXCopyContext
    {
    public:
        static CDirectXCopyContext* Get()
        {
            static auto copyContext = new CDirectXCopyContext();
            return copyContext;
        }
        CDirectXCopyContext();
        ~CDirectXCopyContext();
        ID3D12GraphicsCommandList* GetCommandList() { return m_copyCommandList; }
        ID3D12CommandQueue* GetCommandQueue() { return m_copyQueue; }
        ID3D12CommandAllocator* GetAllocator() { return m_copyCommandAllocator; }
      
        void StartCopyTextureRegion(ID3D12Resource* srcResource, ID3D12Resource* dstResource, int width, int height);
        void EndCopyTextureRegion();
        void StartUpload(ID3D12Resource* resource, size_t bufferSize, void* data);
        void EndUpload();
    private:
        void CreateQueues();
        void CreateCommandList();
        void WaitAndReset();

        ID3D12CommandQueue* m_copyQueue;
        ID3D12CommandAllocator* m_copyCommandAllocator;
        ID3D12GraphicsCommandList* m_copyCommandList;

        ID3D12Resource* m_uploadResource;
        DirectXFence* m_copyFence;

        DirectXCommandBuffer* m_copyCommandBuffer;

        QCore::Delegate<> m_afterCopyCallbacks;
        UINT8* m_mappingAddress;
    };
}

