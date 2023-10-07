#include "directx_graphics_context.h"
#include "directx_texture.h"
#include "window.h"
#include "d3dx12.h"
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d12")

#define DX12_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_1

namespace KNR
{
	CDirectXContext::CDirectXContext()
	{
		m_device = 0;
		m_commandList = 0;
		m_swapChain = 0;
	}
	CDirectXContext::CDirectXContext(WindowDesc* windowHandle)
		: m_window(windowHandle)
	{
		m_device = 0;
		m_commandList = 0;
		m_swapChain = 0;
	}

	CDirectXContext::~CDirectXContext()
	{
		m_device->Release();
		m_device = 0;

		m_commandList->Release();
		m_commandList = 0;

		m_commandQueue->Release();
		m_commandQueue = 0;

		m_computeQueue->Release();
		m_computeQueue = 0;

		for (int i = 0; i < 3; ++i)
		{
			m_commandAllocator[i]->Release();
			m_commandAllocator[i] = 0;
		}

		delete m_blitFence;
		m_blitFence = 0;

		delete m_swapChain;
		m_swapChain = 0;
		
		delete m_frameHeap;
		m_frameHeap = 0;
	}

	void CDirectXContext::Init()
	{

	}

	void CDirectXContext::Init(WindowDesc* windowHandle)
	{
		m_window = windowHandle;

		CreateDevice();
		CreateQueues();
		CreateSwapchain();
		CreateCommandList();
		CreateReservedHeapAllocations();

		m_blitFence = new DirectXFence(false);
		m_frameHeap = new DirectXFrameHeap();
		m_copyCommandBuffer = new DirectXCommandBuffer(KNR::CommandBufferType::graphics);
	}

	void CDirectXContext::SwapBuffers()
	{
		//glfwPollEvents();
	}

	void CDirectXContext::StartBlit(ID3D12Resource* srcResource, ID3D12Resource* dstResource, int width, int height)
	{
		assert(width > 0);
		assert(height > 0);
		m_blitAllocator->Reset();
		m_blitCommandList->Reset(m_blitAllocator, nullptr);

		CD3DX12_TEXTURE_COPY_LOCATION dest(dstResource);
		CD3DX12_TEXTURE_COPY_LOCATION src(srcResource);

		auto preDstBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dstResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
		auto preSrcBarrier = CD3DX12_RESOURCE_BARRIER::Transition(srcResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE);

		m_blitCommandList->ResourceBarrier(1, &preDstBarrier);
		m_blitCommandList->ResourceBarrier(1, &preSrcBarrier);
		CD3DX12_BOX box(0, 0, width, height);
		m_blitCommandList->CopyTextureRegion(&dest, 0, 0, 0, &src, &box);

		auto postDstBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dstResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		auto postSrcBarrier = CD3DX12_RESOURCE_BARRIER::Transition(srcResource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_blitCommandList->ResourceBarrier(1, &postDstBarrier);
		m_blitCommandList->ResourceBarrier(1, &postSrcBarrier);
	}

	void CDirectXContext::EndBlit()
	{
		//Close the command list
		HRESULT hr = m_blitCommandList->Close();
		if (FAILED(hr))
		{
			assert(0);
		}

		ID3D12CommandList* ppCommandLists[] = { m_blitCommandList };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		m_blitFence->Signal();
		m_blitFence->WaitForFenceEvent();
	}

	void CDirectXContext::StartBlitToSwapchain(ID3D12Resource* srcResource, ID3D12Resource* dstResource, int width, int height)
	{
		assert(width > 0);
		assert(height > 0);
		m_blitAllocator->Reset();
		m_blitCommandList->Reset(m_blitAllocator, nullptr);

		CD3DX12_TEXTURE_COPY_LOCATION dest(dstResource);
		CD3DX12_TEXTURE_COPY_LOCATION src(srcResource);

		auto preDstBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dstResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST);
		auto preSrcBarrier = CD3DX12_RESOURCE_BARRIER::Transition(srcResource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE);

		m_blitCommandList->ResourceBarrier(1, &preDstBarrier);
		m_blitCommandList->ResourceBarrier(1, &preSrcBarrier);
		CD3DX12_BOX box(0, 0, width, height);
		m_blitCommandList->CopyTextureRegion(&dest, 0, 0, 0, &src, &box);

		auto postDstBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dstResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
		auto postSrcBarrier = CD3DX12_RESOURCE_BARRIER::Transition(srcResource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_blitCommandList->ResourceBarrier(1, &postDstBarrier);
		m_blitCommandList->ResourceBarrier(1, &postSrcBarrier);
	}

	void CDirectXContext::CreateDevice()
	{
		//set the feature level to DirectX 12.1 to enable all DX12 features.
		//Not all cards support full DX12,  this feature level may need  to be reduced on some cards to 12.0
		IDXGIFactory4* factory;
		IDXGIAdapter* adapter;
		ID3D12Debug* debugInterface;
		ID3D12Debug1* debugInterface1;

		HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
		if (FAILED(hr))
		{
			assert(0);
		}

		debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1));

		//Q_RENDER_INFO("Enabling debug layer");
		debugInterface->EnableDebugLayer();
		debugInterface1->SetEnableGPUBasedValidation(true);


		hr = D3D12CreateDevice(NULL, DX12_FEATURE_LEVEL, IID_PPV_ARGS(&m_device));
		if (FAILED(hr))
		{
			assert(0);
		}

		//Create a DirectX graphics interface factory
		hr = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);
		if (FAILED(hr))
		{
			assert(0);
		}

		//Use factory to create an adaptor for the graphics card
		hr = factory->EnumAdapters(0, &adapter);
		if (FAILED(hr))
		{
			assert(0);
		}

		
		DXGI_ADAPTER_DESC adapterDesc;
		unsigned long long stringLength;

		//Get adapter (video card) description)
		hr = adapter->GetDesc(&adapterDesc);
		if (FAILED(hr))
		{
			assert(0);
		}

		//store the amount of dedicated video card memory in megabytes
		m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		m_systemMemory = (int)(adapterDesc.SharedSystemMemory / 1024 / 1024);
		void* multisampleQualityLevel;

		//Convert the name of the video card to a character array and store it
		int error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
		if (error != 0)
		{

		}
	}

	void CDirectXContext::CreateQueues()
	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
		//Initialize the description of the command queue
		ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));

		//Set up the description of the command queue.
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.NodeMask = 0;

		//Create the command queue
		HRESULT result = m_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_commandQueue);
		if (FAILED(result))
		{
			assert(0);
		}

		D3D12_COMMAND_QUEUE_DESC computeQueueDesc;
		//Initialize the description of the command queue
		ZeroMemory(&computeQueueDesc, sizeof(computeQueueDesc));

		//Set up the description of the command queue.
		computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		computeQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		computeQueueDesc.NodeMask = 0;

		//Create the command queue
		result = m_device->CreateCommandQueue(&computeQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_computeQueue);
		if (FAILED(result))
		{
			assert(0);
		}
	}

	void CDirectXContext::CreateSwapchain()
	{
		//Liam fix - need IWindow size here (width and height)
		int width = m_window->width;
		int height = m_window->height;
		m_swapChain = new DirectXSwapchain(m_window->hwnd, m_window->width, m_window->height);
	}

	void CDirectXContext::CreateCommandList()
	{
		HRESULT result;
		//create a command allocator
		for (int i = 0; i < 3; i++)
		{
			result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_commandAllocator[i]);
			if (FAILED(result))
			{
				assert(0);
			}
		}

		//Create a basic command list
		result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[0], NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_commandList);
		if (FAILED(result))
		{
			assert(0);
		}

		//Close immediately
		result = m_commandList->Close();
		if (FAILED(result))
		{
			assert(0);
		}

		//Create a blit allocator
		result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_blitAllocator);
		if (FAILED(result))
		{
			assert(0);
		}

		//Create a blit command list
		result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_blitAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_blitCommandList);
		if (FAILED(result))
		{
			assert(0);
		}

		result = m_blitCommandList->Close();
		if (FAILED(result))
		{
			assert(0);
		}


		//Create a imGui allocator
		result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_imGuiCommandAllocator);
		if (FAILED(result))
		{
			assert(0);
		}

		//Create an imgui command list
		result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_imGuiCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_imGuiCommandList);
		if (FAILED(result))
		{
			assert(0);
		}

		result = m_imGuiCommandList->Close();
		if (FAILED(result))
		{
			assert(0);
		}
	}

	void CDirectXContext::CreateReservedHeapAllocations()
	{
		m_imguiHeap.Create(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 200, true);
		m_imguiHeap.IncrementHeapIndex(); //(For this heap, ImGui needs the first index for a font so lets increment it here as a hack for that)

		m_heapReservationMaxSizes[(int)BindlessHeapRegion::TEXTURE1D] = 20;
		m_heapReservationMaxSizes[(int)BindlessHeapRegion::TEXTURE2D] = 1000;
		m_heapReservationMaxSizes[(int)BindlessHeapRegion::TEXTURE3D] = 20;
		m_heapReservationMaxSizes[(int)BindlessHeapRegion::CUBEMAPS]  = 30;

		for (int i = (int)BindlessHeapRegion::BINDLESSSTART; i < (int)BindlessHeapRegion::BINDLESSEND; ++i)
		{
			m_heapReservationUsed[i] = 0;
			m_heapReservationSlots[i].Create(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_heapReservationMaxSizes[i], true);
		}
	}


	DirectXDescriptorHandleBlock CDirectXContext::ReserveDescriptorHandle(ID3D12Resource* resource, BindlessHeapRegion region, D3D12_SHADER_RESOURCE_VIEW_DESC srv, RENDERERID& renderId)
	{
		uint32_t regionLimit = m_heapReservationMaxSizes[(int)region];

		if (m_heapReservationUsed[(int)region] == regionLimit)
		{
			//We've exceeded our heap limit, increase the sizes of the heap limits to get this to work l m a o 
			assert(0);
		}

		DirectXHeap& regionHeap = m_heapReservationSlots[(int)region];

		renderId = regionHeap.GetCurrentHeapIndex();

		DirectXDescriptorHandleBlock descriptorBlock = {};
		descriptorBlock.cpuHandle = regionHeap.handleCPU(renderId);
		descriptorBlock.gpuHandle = regionHeap.handleGPU(renderId);

		m_device->CreateShaderResourceView(resource, &srv, descriptorBlock.cpuHandle);
		regionHeap.IncrementHeapIndex();

		m_heapReservationUsed[(int)region] = renderId + 1;

		return descriptorBlock;
	}

}