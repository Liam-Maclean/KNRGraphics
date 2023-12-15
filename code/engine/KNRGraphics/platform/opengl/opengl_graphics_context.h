#pragma once
#define OpenglContext (*KNR::CVulkanContext::Get())
#include "graphics_context.h"
#include "window.h"
#include "render_types.h"
#include "opengl_fence.h"
#include "opengl_command_buffer.h"
#include "opengl_swapchain.h"

namespace KNR
{
    class COpenglContext :
        public CGraphicsContext
    {
    public:
        static COpenglContext* Get()
        {
            static auto renderContext = new COpenglContext();
            return renderContext;
        }

        COpenglContext();
        COpenglContext(const WindowDesc& windowDesc);
        ~COpenglContext();

        virtual void Init(const WindowDesc& windowDesc);
        virtual void Init() override;
        virtual void SwapBuffers() override;

        //inline VkDevice GetDevice() { return m_device; }
        //inline VkPhysicalDevice GetPhysicalDevice() { return m_physicalDevice; }
        //inline VkInstance GetInstance() { return m_instance; }
        //inline VkSwapchainKHR GetSwapchain() { return m_swapChain->GetSwapchain(); }
        //inline VkQueue GetCommandQueue() { return m_graphicsQueue; }
        //inline VkQueue GetComputeQueue() { return m_computeQueue; }
        //inline VulkanCommandBuffer* GetCopyCommandBuffer() { return m_copyCommandBuffer; }
        //inline uint32_t GetComputeQueueIndex() { return m_computeQueueFamilyIdx; }
        //inline uint32_t GetCopyQueueIndex() { return m_copyQueueFamilyIdx; }
        //inline uint32_t GetGraphicsQueueIndex() { return m_graphicsQueueFamilyIdx; }
        //inline uint32_t GetPresentQueueIndex() { return m_presentQueueFamilyIdx; }
        //inline VmaAllocator GetVMAAllocator() { return m_vmaAllocator; }
    private:
       
        void CreateInstance();
        void CreateDevice();
        void CreateQueues();
        void CreateSwapchain();
        void CreateReservedHeapAllocations();
     
        WindowDesc m_window;

        OpenglSwapchain* m_swapChain;
        OpenglCommandBuffer* m_copyCommandBuffer;
        OpenglCommandBuffer* m_blitCommandList;

        HardwareGPUTelemetry m_gpuTelemetry;

        uint32_t m_gpuCount;
        unsigned int m_videoCardMemory;
        unsigned int m_systemMemory;
        char m_videoCardDescription[128];
    };
}

