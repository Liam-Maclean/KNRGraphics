#include "directx12_graphics_context.h"
#include "directx12_swapchain.h"
#include "logger/logger.h"

namespace KNR
{
	DirectX12Swapchain::DirectX12Swapchain(void* windowPtr, void* instance, const int screenWidth, const int screenHeight)
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		IDXGISwapChain* swapChain;
		IDXGIFactory4* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		DXGI_MODE_DESC* displayModeList;
		unsigned int numerator, denominator, i, numModes;
		int width = screenWidth;
		int height = screenHeight;

		//Grab the HWND window from glfw
		HWND window = static_cast<HWND>(windowPtr);
		
		//Create a DirectX graphics interface factory
		HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);
		if (FAILED(result))
		{
			assert(0);
		}

		//Use factory to create an adaptor for the graphics card
		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result))
		{
			assert(0);
		}

		//enumerate primary adapter output (monitor)
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result))
		{
			assert(0);
		}

		//Get the number of modes that fit the R8G8B8A8_UNORM display format for the monitor
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if (FAILED(result))
		{
			assert(0);
		}

		//Create a list to hold the possible displ;ay modes for this monitor/videocard combination
		displayModeList = new DXGI_MODE_DESC[numModes];
		if (!displayModeList)
		{
			assert(0);
		}

		//Now fill the display mode list structures
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if (FAILED(result))
		{
			assert(0);
		}

		//Now go through all the display modes and find the one that matches the screen height and width
		//when a match is found, store the numerator and demoninator of the refresh rate of the monitor
		for (i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				if (displayModeList[i].Width == (unsigned int)screenWidth)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
					break;
				}
			}
		}


		//Initialize swap chain description
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		//set up swapchain to use double buffering
		//liam fix - Come back to this later. hard coded buffer count is 3
		swapChainDesc.BufferCount = 3;

		//set t he height and width of the back buffers in the swap chain.
		swapChainDesc.BufferDesc.Height = screenHeight;
		swapChainDesc.BufferDesc.Width = screenWidth;

		//set a regular 32 bit surface for the back buffers;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		//set the usage of the back buffers to be render target outputs
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		//set the swap chain to discard the previous buffer contents after swapping
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		//Set the handle for the window to render to
		swapChainDesc.OutputWindow = window;

		//liam fix - This is set to only work in windowed for now
		if (!m_fullscreen)
		{
			swapChainDesc.Windowed = true;
		}
		else
		{
			swapChainDesc.Windowed = false;
		}

		if (m_vsync_enabled)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		//Turn multisampling off
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		//Set the scan line ordering and scaling to unspecified
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		//Dont set advanced flags 
		swapChainDesc.Flags = 0;

		IDXGISwapChain3* swapChain3;

		result = factory->CreateSwapChain(DirectX12Context.GetCommandQueue(), &swapChainDesc, &swapChain);
		if (FAILED(result))
		{
			KNT_ERROR("Failed to make swapchain");
			assert(0);
		}

		result = swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_swapChain);
		if (FAILED(result))
		{
			KNT_ERROR("Failed to query swapchain interface");

			assert(0);
		}

		//release the display mode list
		delete[] displayModeList;
		displayModeList = 0;

		//release the adapter output
		adapterOutput->Release();
		adapterOutput = 0;

		//release the adapter
		adapter->Release();
		adapter = 0;

		factory->Release();
		factory = 0;

		swapChain = 0;
	}

	DirectX12Swapchain::DirectX12Swapchain(GLFWwindow* hwnd, const int screenWidth, const int screenHeight)
	{
	}

	DirectX12Swapchain::~DirectX12Swapchain()
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}
}
