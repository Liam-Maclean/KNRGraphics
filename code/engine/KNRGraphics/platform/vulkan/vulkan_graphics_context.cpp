#include "vulkan_graphics_context.h"
#include "vulkan_texture.h"
#include "vulkan_util.h"
#include "window.h"
#include "logger/logger.h"

#define VULKAN_FEATURE_LEVEL 0

namespace KNR
{
	//API call for debugcallbacks
	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type, uint64_t src_obj, size_t location, int32_t msg_code, const char* layer_prefix, const char* msg, void* user_data)
	{
		if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		{
			KNT_INFO("PERFORMANCE: ");
		}
		if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
			KNT_INFO("WARNING: ");
		}
		if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			KNT_INFO("PERFORMANCE: ");
		}
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			KNT_ERROR("ERROR: ");
		}
		if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		{
			KNT_DEBUG("ERROR: ");
		}

		KNT_DEBUG("@[%s]  %s", layer_prefix, msg);
		return false;
	}


	CVulkanContext::CVulkanContext()
	{
		m_device = 0;
		m_swapChain = 0;
	}
	CVulkanContext::CVulkanContext(const WindowDesc& windowDesc)
		: m_window(windowDesc)
	{
		m_device = 0;
		m_swapChain = 0;
	}

	CVulkanContext::~CVulkanContext()
	{
		vkDestroyDevice(m_device, nullptr);
		m_device = 0;

		vkDestroyInstance(m_instance, nullptr);
		m_instance = 0;

		delete m_swapChain;
		m_swapChain = 0;
	}

	void CVulkanContext::Init()
	{

	}

	void CVulkanContext::Init(const WindowDesc& windowDesc)
	{
		m_window = windowDesc;

		CreateInstance();
		CreateDevice();
		CreateQueues();
		CreateSwapchain();
		CreateVMAAllocator();

		m_copyCommandBuffer = new VulkanCommandBuffer(KNR::CommandBufferType::graphics);
	}

	void CVulkanContext::SwapBuffers()
	{
		//glfwPollEvents();
	}

	void CVulkanContext::StartBlit(VkBuffer srcResource, VkBuffer dstResource, int width, int height)
	{
		//TODO
	}

	void CVulkanContext::EndBlit()
	{
		//TODO
	}

	void CVulkanContext::StartBlitToSwapchain(VkBuffer srcResource, VkBuffer dstResource, int width, int height)
	{
		//TODO
	}

	bool CVulkanContext::IsDeviceSuitable(VkPhysicalDevice device)
	{
		bool extensionsSupported = CheckDeviceExtentionSupport(device);

		return extensionsSupported;
	}

	//Method to find and filter the queue families of the device we have chosen
	void CVulkanContext::QueryFamilyQueueIndices(VkPhysicalDevice device)
	{
		//query device for family queue support properties
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		//loop through queue families to find specific queue types and flags
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 )
			{
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					m_graphicsQueueFamilyIdx = i;
					m_presentQueueFamilyIdx = i;
				}
				else if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					m_computeQueueFamilyIdx = i;
				}
				else if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
				{
					m_copyQueueFamilyIdx = i;
				}
			}
			i++;
		}
	}

	//Method to check device extention support (lists and loops through)
	bool CVulkanContext::CheckDeviceExtentionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(m_requiredDeviceExtensions.begin(), m_requiredDeviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	void CVulkanContext::CreateVMAAllocator()
	{
		uint32_t apiVersion;
		vkEnumerateInstanceVersion(&apiVersion);

		VmaAllocatorCreateInfo allocator_info = {};
		allocator_info.physicalDevice = m_physicalDevice;
		allocator_info.device = m_device;
		allocator_info.instance = m_instance;
		allocator_info.vulkanApiVersion = apiVersion;

		Util::ErrorCheck(vmaCreateAllocator(&allocator_info, &m_vmaAllocator));
	}


	void CVulkanContext::CreateDebugCallback()
	{
		//Set up the debug callback information structure with the API call function and flags
		m_debugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		m_debugCallbackCreateInfo.pfnCallback = VulkanDebugCallback;
		m_debugCallbackCreateInfo.flags =
			VK_DEBUG_REPORT_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_ERROR_BIT_EXT |
			VK_DEBUG_REPORT_DEBUG_BIT_EXT |
			0;

		//pushback the layer we want to use as our validation layers for the instance
		//_instance_layers.push_back("VK_LAYER_RENDERDOC_Capture");
		//_instance_layers.push_back("VK_LAYER_LUNARG_standard_validation");
		//_instance_layers.push_back("VK_LAYER_LUNARG_object_tracker");
		//_instance_layers.push_back("VK_LAYER_NV_optimus");
		//_instance_layers.push_back("VK_LAYER_NV_nomad");
		//_instance_layers.push_back("VK_LAYER_NV_nsight");
		 
		//pushback the instance extention debug reports
		m_instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	void CVulkanContext::CreateDevice()
	{
		//get the number of GPU devices on the computer
		vkEnumeratePhysicalDevices(m_instance, &m_gpuTelemetry.gpuCount, nullptr);

		//store count
		std::vector<VkPhysicalDevice> gpu_list(m_gpuTelemetry.gpuCount);

		//call again to get a list of the GPUs
		vkEnumeratePhysicalDevices(m_instance, &m_gpuTelemetry.gpuCount, gpu_list.data());

		//Find out if a device is suitable to be chosen for our project
		for (const auto& device : gpu_list)
		{
			if (IsDeviceSuitable(device))
			{
				m_physicalDevice = device;
				break;
			}
		}

		//get the physical device properties of the GPU we're accessing
		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_deviceProperties);
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_deviceMemoryProperties);

		KNT_INFO("");
		KNT_INFO("Graphics Device: {0}", (std::string)m_deviceProperties.deviceName);
		KNT_INFO("Device driver version: {0}", m_deviceProperties.driverVersion);
		KNT_INFO("Device API version: {0}", m_deviceProperties.apiVersion);
		KNT_INFO("Vendor ID: {0}", m_deviceProperties.vendorID);
		KNT_INFO("");
		KNT_INFO("Memory Properties");
		KNT_INFO("Device Local Memory (VRAM): {0}", (m_deviceMemoryProperties.memoryHeaps[0].size / 1024 / 1024));
		KNT_INFO("Host Coherant memory (RAM): {0}", (m_deviceMemoryProperties.memoryHeaps[1].size / 1024 / 1024));
		KNT_INFO("");

		//get the number of layers from the instance
		uint32_t layerCount = 0;

		//do once to get the layer count
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		//create a vector with the size of layers the instance has
		std::vector<VkLayerProperties> layerPropertyList(layerCount);

		//call twice to store the layer instance amount
		vkEnumerateInstanceLayerProperties(&layerCount, layerPropertyList.data());

		//write out all layers found using iostream
		KNT_INFO("Instance Layers: ");
		for (auto& i : layerPropertyList)
		{
			KNT_INFO((std::string)i.layerName + ": " + (std::string)i.description);
		}

		QueryFamilyQueueIndices(m_physicalDevice);
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { m_graphicsQueueFamilyIdx, m_copyQueueFamilyIdx, m_computeQueueFamilyIdx, m_presentQueueFamilyIdx };

		//QUEUE DEVICE INFO FOR THE GRAPHICS FAMILY QUEUE WE ARE INTERSTED IN
		float queue_priorities[]{ 1.0f };
		for (int queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo device_queue_create_info{};
			device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_info.queueFamilyIndex = queueFamily;
			device_queue_create_info.queueCount = 1;
			device_queue_create_info.pQueuePriorities = queue_priorities;
			queueCreateInfos.push_back(device_queue_create_info);
		}

		//Device info for the device we are creating 
		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		device_create_info.pQueueCreateInfos = queueCreateInfos.data();
		device_create_info.enabledExtensionCount = static_cast<uint32_t>(m_requiredDeviceExtensions.size());
		device_create_info.ppEnabledExtensionNames = m_requiredDeviceExtensions.data();

		//Create a device using the vkCreateDevice function, and error check that we have a device 
		Util::ErrorCheck(vkCreateDevice(m_physicalDevice, &device_create_info, nullptr, &m_device));
	}

	void CVulkanContext::CreateInstance()
	{
		//checking available extentions for debug purposes:
		uint32_t extention_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extention_count, nullptr);
		std::vector<VkExtensionProperties> extentions(extention_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extention_count, extentions.data());

		//output the available instance extentions to the console window
		for (auto i = 0; i < extentions.size(); i++)
		{
			KNT_INFO("Available Extentions: " +  (std::string)extentions[i].extensionName);
		}

		//add glfw extentions to extention list
		//glfwExtentions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);
		//for (auto i = 0; i < glfwExtentionCount; i++)
		//{
		//
		//	_instance_extentions.push_back(glfwExtentions[i]);
		//	std::cout << "GLFW EXTENTION ADDED: " << glfwExtentions[i] << std::endl;
		//}
		//std::cout << std::endl;

		//set up the application information for the instance (API version, program application version and name)
		VkApplicationInfo application_info{};
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.apiVersion = VK_API_VERSION_1_0;
		application_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
		application_info.pApplicationName = "KNR Graphics Engine";

		//Set up the instance create information (Validation layers, application layers, extention counts)
		VkInstanceCreateInfo instance_create_info{};
		instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_create_info.pApplicationInfo = &application_info;
		instance_create_info.enabledLayerCount = m_instanceLayers.size();
		instance_create_info.ppEnabledLayerNames = m_instanceLayers.data();
		instance_create_info.enabledExtensionCount = m_instanceExtensions.size();
		instance_create_info.ppEnabledExtensionNames = m_instanceExtensions.data();
		instance_create_info.pNext = &m_debugCallbackCreateInfo;

		//Create the instance using vkCreateInstance function, and error check
		Util::ErrorCheck(vkCreateInstance(&instance_create_info, nullptr, &m_instance));

		KNT_INFO("Vulkan Instance Created");
	}

	void CVulkanContext::CreateQueues()
	{
		vkGetDeviceQueue(m_device, m_graphicsQueueFamilyIdx, 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, m_copyQueueFamilyIdx, 0, &m_copyQueue);
		vkGetDeviceQueue(m_device, m_computeQueueFamilyIdx, 0, &m_computeQueue);
		vkGetDeviceQueue(m_device, m_presentQueueFamilyIdx, 0, &m_presentQueue);
	}

	void CVulkanContext::CreateSwapchain()
	{
		//Liam fix - need IWindow size here (width and height)
		int width = m_window.width;
		int height = m_window.height;
		//m_swapChain = new VulkanSwapchain(m_window.hwnd, m_window.instance, m_window.width, m_window.height);
	}
}