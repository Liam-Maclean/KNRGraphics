#pragma once
#define DirectX11Context (*KNR::CDirectX11Context::Get())
#define VK_USE_PLATFORM_WIN32_KHR 1
#include "graphics_context.h"
#include "window.h"
#include "render_types.h"
#include "directx11_fence.h"
#include "directx11_command_buffer.h"
#include "directx11_swapchain.h"

namespace KNR
{
    class VulkanCommandBuffer;
    class CDirectX11Context :
        public CGraphicsContext
    {
    public:
        static CDirectX11Context* Get()
        {
            static auto renderContext = new CDirectX11Context();
            return renderContext;
        }

        CDirectX11Context();
        CDirectX11Context(const WindowDesc& windowDesc);
        ~CDirectX11Context();

        virtual void Init(const WindowDesc& windowDesc);
        virtual void Init() override;
        virtual void SwapBuffers() override;

        inline DirectX11CommandBuffer* GetCopyCommandBuffer() { return m_copyCommandBuffer; }
        inline uint32_t GetComputeQueueIndex() { return m_computeQueueFamilyIdx; }
        inline uint32_t GetCopyQueueIndex() { return m_copyQueueFamilyIdx; }
        inline uint32_t GetGraphicsQueueIndex() { return m_graphicsQueueFamilyIdx; }
        inline uint32_t GetPresentQueueIndex() { return m_presentQueueFamilyIdx; }
    private:
        
        void CreateDevice();
        void CreateQueues();
        void CreateSwapchain();
        void CreateReservedHeapAllocations();

        WindowDesc m_window; 

        DirectX11Swapchain* m_swapChain;
        DirectX11CommandBuffer* m_copyCommandBuffer;
        DirectX11CommandBuffer* m_blitCommandList;

        HardwareGPUTelemetry m_gpuTelemetry;

        //Vulkan requires a ridiculous amount of shit to set up 
        uint32_t m_computeQueueFamilyIdx;
        uint32_t m_graphicsQueueFamilyIdx;
        uint32_t m_presentQueueFamilyIdx;
        uint32_t m_copyQueueFamilyIdx;

        std::vector<const char*> m_instanceLayers;
        std::vector<const char*> m_instanceExtensions;

        uint32_t m_gpuCount;
        unsigned int m_videoCardMemory;
        unsigned int m_systemMemory;
        char m_videoCardDescription[128];
    };
}

