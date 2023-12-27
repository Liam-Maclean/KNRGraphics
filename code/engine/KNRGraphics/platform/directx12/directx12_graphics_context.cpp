#include "directx12_graphics_context.h"
#include "../KNRShaders/KNR_RootSignature.h"
#include "directx12_texture.h"
#include "logger/logger.h"
#include "window.h"
#include "d3dx12.h"
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d12")

#define DX12_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_1

namespace KNR
{
	CDirectX12Context::CDirectX12Context()
	{
		m_device = 0;
		m_commandList = 0;
		m_swapChain = 0;
	}
	CDirectX12Context::CDirectX12Context(const WindowDesc& windowHandle)
		:m_window(windowHandle)
	{
		m_device = 0;
		m_commandList = 0;
		m_swapChain = 0;
	}

	CDirectX12Context::~CDirectX12Context()
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

		delete m_swapChain;
		m_swapChain = 0;
		
		delete m_frameHeap;
		m_frameHeap = 0;
	}

	void CDirectX12Context::Init()
	{

	}

	void CDirectX12Context::Init(const WindowDesc& windowHandle)
	{
		m_window = windowHandle;
		
		Logger::Init();


		CreateDevice();
		CreateQueues();
		CreateCommandList();
		CreateBindlessHeapReservations();

		CD3DX12_DESCRIPTOR_RANGE1 DescRange[7];
		DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, -1, 0, 0); // Unbound, space 0
		DescRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, -1, 0, 1); // Unbound, space 1
		DescRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, -1, 0, 2); // Unbound, space 2
		DescRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, -1, 0, 3); // Unbound, space 3
		DescRange[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, -1, 0, 4); // Unbound, space 4
		DescRange[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, -1, 0, 5); // Unbound, space 5
		DescRange[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, -1, 0, 6); // Unbound, space 6

		CD3DX12_ROOT_PARAMETER1 RP[9];
		RP[0].InitAsDescriptorTable(1, &DescRange[0]); // Texture1D
		RP[1].InitAsDescriptorTable(1, &DescRange[1]); // Texture2D
		RP[2].InitAsDescriptorTable(1, &DescRange[2]); // Texture2DArray
		RP[3].InitAsDescriptorTable(1, &DescRange[3]); // Texture3D
		RP[4].InitAsDescriptorTable(1, &DescRange[4]); // TextureCubemap
		RP[5].InitAsDescriptorTable(1, &DescRange[5]); // TextureCubemapArray
		RP[6].InitAsDescriptorTable(1, &DescRange[6]); // RawByteData (CBV)
		RP[7].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_ALL);
		RP[8].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_STATIC_SAMPLER_DESC StaticSamplers[1];
		StaticSamplers[0].Init(0, D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT); // s3
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootSig(9,RP, 1, StaticSamplers);
		ID3DBlob* pSerializedRootSig;

		HRESULT hr = D3D12SerializeVersionedRootSignature(&RootSig, &pSerializedRootSig, nullptr);
		if (FAILED(hr))
		{
			KNT_ERROR("Failed to create root signature");
			assert(0);
		}

		hr = m_device->CreateRootSignature(0, pSerializedRootSig->GetBufferPointer(), pSerializedRootSig->GetBufferSize(), IID_PPV_ARGS(&m_coreRootSignature));
		if (FAILED(hr))
		{
			KNT_ERROR("Failed t ocreate root signature");
			assert(0);
		}

		m_swapChain = new DirectX12Swapchain(m_window.hwnd, m_window.instance, m_window.width, m_window.height);
		m_frameHeap = new DirectX12FrameHeap();
		m_copyCommandBuffer = new DirectX12CommandBuffer(KNR::CommandBufferType::Graphics);
	}

	void CDirectX12Context::SwapBuffers()
	{
		//glfwPollEvents();
	}

	void CDirectX12Context::CreateDevice()
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

	void CDirectX12Context::CreateQueues()
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

	void CDirectX12Context::CreateCommandList()
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
	}

	void CDirectX12Context::CreateBindlessHeapReservations()
	{
		m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::BindlessTexture1D] = 20;
		m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::BindlessTexture2D] = 1000;
		m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::BindlessTexture2DArray] = 10;
		m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::BindlessTexture3D] = 20;
		m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::BindlessCubemap]  = 30;
		m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::BindlessCubemapArray] = 30;
		m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::BindlessConstant] = 30;
		m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::RenderTarget] = 30;
		m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::DepthStencil] = 30;

		//Bindless heaps
		for (int i = (int)ReservedHeapRegion::BindlessStart; i < (int)ReservedHeapRegion::BindlessEnd; ++i)
		{
			m_bindlessReservationHeapUsed[i] = 0;
			m_bindlessReservationHeaps[i].Create(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_bindlessReservationHeapSizes[i], true);
		}

		//Other heaps
		m_bindlessReservationHeapUsed[(int)ReservedHeapRegion::RenderTarget] = 0;
		m_bindlessReservationHeapUsed[(int)ReservedHeapRegion::DepthStencil] = 0;
		m_bindlessReservationHeaps[(int)ReservedHeapRegion::RenderTarget].Create(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::RenderTarget]);
		m_bindlessReservationHeaps[(int)ReservedHeapRegion::DepthStencil].Create(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_bindlessReservationHeapSizes[(int)ReservedHeapRegion::DepthStencil]);
	}


	DirectX12DescriptorHandleBlock CDirectX12Context::ReserveDescriptorHandle(ReservedHeapRegion region)
	{
		uint32_t regionLimit = m_bindlessReservationHeapSizes[(int)region];

		if (m_bindlessReservationHeapUsed[(int)region] == regionLimit)
		{
			KNT_ERROR("HEAP LIMIT EXCEEDED, INCREASE THE SIZE OF THE HEAP RESERVATIONS")
			assert(0);
		}

		DirectX12Heap& regionHeap = m_bindlessReservationHeaps[(int)region];

		//Create a descriptor block
		DirectX12DescriptorHandleBlock descriptorBlock = {};
		descriptorBlock.bindlessIndexID = regionHeap.GetCurrentHeapIndexAndIncrement();
		descriptorBlock.cpuHandle = regionHeap.handleCPU(descriptorBlock.bindlessIndexID);
		descriptorBlock.gpuHandle = regionHeap.handleGPU(descriptorBlock.bindlessIndexID);

		//Increase the usage counter
		m_bindlessReservationHeapUsed[(int)region] = descriptorBlock.bindlessIndexID + 1;

		return descriptorBlock;
	}

	void CDirectX12Context::CreateSRV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_SHADER_RESOURCE_VIEW_DESC srv)
	{
		m_device->CreateShaderResourceView(resource, &srv, descriptorHandle.cpuHandle);
	}

	void CDirectX12Context::CreateCBV(const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_CONSTANT_BUFFER_VIEW_DESC cbv)
	{
		m_device->CreateConstantBufferView(&cbv, descriptorHandle.cpuHandle);
	}

	void CDirectX12Context::CreateUAV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC uav)
	{
		m_device->CreateUnorderedAccessView(resource, nullptr, &uav, descriptorHandle.cpuHandle);
	}

	void CDirectX12Context::CreateRTV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_RENDER_TARGET_VIEW_DESC rtv)
	{
		m_device->CreateRenderTargetView(resource, &rtv, descriptorHandle.cpuHandle);
	}

	void CDirectX12Context::CreateBackbufferRTV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle)
	{
		m_device->CreateRenderTargetView(resource, NULL, descriptorHandle.cpuHandle);
	}

	void CDirectX12Context::CreateDSV(ID3D12Resource* resource, const DirectX12DescriptorHandleBlock& descriptorHandle, const D3D12_DEPTH_STENCIL_VIEW_DESC dsv)
	{
		m_device->CreateDepthStencilView(resource, &dsv, descriptorHandle.cpuHandle);
	}
}