#pragma once
#define VulkanContext (*KNR::CVulkanContext::Get())
#define VK_USE_PLATFORM_WIN32_KHR 1
#include "graphics_context.h"
#include "window.h"
#include "render_types.h"
#include "vulkan_fence.h"
#include "vulkan_command_buffer.h"
#include "vulkan_swapchain.h"
#include "vulkan/vulkan.h"

namespace KNR
{
    class CVulkanContext :
        public CGraphicsContext
    {
    public:
        static CVulkanContext* Get()
        {
            static auto renderContext = new CVulkanContext();
            return renderContext;
        }

        CVulkanContext();
        CVulkanContext(WindowDesc* windowHandle);
        ~CVulkanContext();

        void Init(WindowDesc* windowHandle);
        virtual void Init() override;
        virtual void SwapBuffers() override;

        inline VkDevice GetDevice() { return m_device; }
        inline VkPhysicalDevice GetPhysicalDevice() { return m_physicalDevice; }
        inline VkInstance GetInstance() { return m_instance; }
        inline VkSwapchainKHR GetSwapchain() { return m_swapChain->GetSwapchain(); }
        inline VkQueue GetCommandQueue() { return m_graphicsQueue; }
        inline VkQueue GetComputeQueue() { return m_computeQueue; }
        inline VulkanCommandBuffer* GetCopyCommandBuffer() { return m_copyCommandBuffer; }
        inline uint32_t GetComputeQueueIndex() { return m_computeQueueFamilyIdx; }
        inline uint32_t GetCopyQueueIndex() { return m_copyQueueFamilyIdx; }
        inline uint32_t GetGraphicsQueueIndex() { return m_graphicsQueueFamilyIdx; }
        inline uint32_t GetPresentQueueIndex() { return m_presentQueueFamilyIdx; }


        void StartBlit(VkBuffer srcResource, VkBuffer dstResource, int width, int height);
        void EndBlit();

        void StartBlitToSwapchain(VkBuffer srcResource, VkBuffer dstResource, int width, int height);
    private:
        
        bool CheckDeviceExtentionSupport(VkPhysicalDevice device);
        void QueryFamilyQueueIndices(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device);
        void CreateDebugCallback();
        void CreateInstance();
        void CreateDevice();
        void CreateQueues();
        void CreateSwapchain();
        void CreateReservedHeapAllocations();

        //Device extentions
        const std::vector<const char*> m_requiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_dynamic_rendering
        };

        WindowDesc* m_window;

        //Vulkan Handles
        VkDevice m_device;
        VkPhysicalDevice m_physicalDevice;
        VkInstance m_instance;
        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;
        VkQueue m_computeQueue;
        VkQueue m_copyQueue;

        VkPhysicalDeviceProperties m_deviceProperties = {};
        VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties = {};


        VulkanSwapchain* m_swapChain;
        VulkanCommandBuffer* m_copyCommandBuffer;
        VulkanCommandList* m_blitCommandList;

        HardwareGPUTelemetry m_gpuTelemetry;


        //Debug reporting callbacks
        VkDebugReportCallbackCreateInfoEXT m_debugCallbackCreateInfo = {};
        VkDebugReportCallbackEXT m_debugCallbackReport = VK_NULL_HANDLE;

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

