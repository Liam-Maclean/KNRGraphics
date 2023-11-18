#include "directx12_texture.h"
#include "directx12_graphics_context.h"
#include "directx12_framebuffer.h"
#include "directx12_util.h"
#include <d3d12.h>
#include "d3dx12.h"

namespace KNR
{
	Texture2D* Texture2D::Create(const TextureDescriptor& desc)
	{
		return new DirectX12Texture2D(desc);
	}

	DirectX12Texture2D::DirectX12Texture2D(const TextureDescriptor& desc)
	{
		if (desc.textureType == TextureType::ASSET)
		{
			m_data = stbi_load(desc.textureAsset.path, &m_width, &m_height, &m_channels, 4);
			assert(m_data, "Failed to load image!");
		}
		else if (desc.textureType == TextureType::FRAMEBUFFER)
		{
			FramebufferSpecification* framebufferSpec = desc.textureFramebuffer.framebufferSpec;
			FramebufferTextureSpecification* framebufferTextureSpec = desc.textureFramebuffer.framebufferTextureSpec;
		
			m_width = framebufferSpec->Width;
			m_height = framebufferSpec->Height;

			assert(m_width > 0);
			assert(m_height > 0);

			//Create a default resource description for the texture
			//Liam fix - Lets deal with resource descriptions later
			ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());

			//we need to handle depth texture buffers slightly differently
			if (Util::IsDepthFormat(framebufferTextureSpec->textureFormat))
			{
				m_textureDSVDesc = {};
				m_textureDSVDesc.Format = Util::GetDXGIFormatType(framebufferTextureSpec->textureFormat);
				m_textureDSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				m_textureDSVDesc.Flags = D3D12_DSV_FLAG_NONE;

				D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
				depthOptimizedClearValue.Format = Util::GetDXGIFormatType(desc.textureFramebuffer.framebufferTextureSpec->textureFormat);
				depthOptimizedClearValue.DepthStencil.Depth = 0.0f;
				depthOptimizedClearValue.DepthStencil.Stencil = 0xff;

				auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				auto textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, framebufferSpec->Width, framebufferSpec->Height, 1, 0, framebufferSpec->Samples, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
				HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &depthOptimizedClearValue, IID_PPV_ARGS(&m_textureResource));
				if (FAILED(hr))
				{
					assert(0);
				}
				m_textureResource->SetName(desc.debugName);
				
				m_textureSRVDesc = {};
				m_textureSRVDesc.Format = Util::GetDXGIFormatType(framebufferTextureSpec->textureFormat);
				m_textureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				m_textureSRVDesc.Texture2D.MipLevels = 1;
				m_textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				DirectX12Context.ReserveDescriptorHandle(m_textureResource, BindlessHeapRegion::TEXTURE2D, m_textureSRVDesc, m_rendererID);
			}
			//Standard RTVC 
			else
			{
				D3D12_RESOURCE_DESC resourceDesc = {};
				resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				resourceDesc.Alignment = 0;
				resourceDesc.Width = framebufferSpec->Width;
				resourceDesc.Height = framebufferSpec->Height;
				resourceDesc.DepthOrArraySize = 1;
				resourceDesc.MipLevels = 1;
				resourceDesc.Format = Util::GetDXGIFormatType(framebufferTextureSpec->textureFormat);
				resourceDesc.SampleDesc.Count = framebufferSpec->Samples;
				resourceDesc.SampleDesc.Quality = 0;
				resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

				D3D12_CLEAR_VALUE clearValue = {};
				clearValue.Color[0] = 0.0f;
				clearValue.Color[1] = 0.0f;
				clearValue.Color[2] = 0.0f;
				clearValue.Color[3] = 1.0f;
				clearValue.Format = resourceDesc.Format;
				auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearValue, IID_PPV_ARGS(&m_textureResource));
				if (FAILED(hr))
				{
					assert(0);
				}

				m_textureResource->SetName(desc.debugName);

				////Populate the SRV descriptor
				m_textureSRVDesc = {};
				ZeroMemory(&m_textureSRVDesc, sizeof(m_textureSRVDesc));
				m_textureSRVDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
				m_textureSRVDesc.Texture2D.MostDetailedMip = 0;
				m_textureSRVDesc.Format = resourceDesc.Format;
				m_textureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				m_textureSRVDesc.TextureCube.MostDetailedMip = 0;
				m_textureSRVDesc.TextureCube.MipLevels = 1;
				m_textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				//Populate the RTV descriptor
				m_textureRTVDesc = {};
				ZeroMemory(&m_textureRTVDesc, sizeof(m_textureRTVDesc));
				m_textureRTVDesc.Format = resourceDesc.Format;
				m_textureRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				m_textureRTVDesc.Texture2D.MipSlice = 0;
				m_textureRTVDesc.Texture2D.PlaneSlice = 0;
				
				////Place it into an ImGui Heap for now until I discuss an alternative
				DirectX12Heap* heap = DirectX12Context.GetImGuiHeap();
				//Create a shader resource (FOR IMGUI) and place it at the top of the index and increment the heap index, 
				//return the renderer ID for the heap so we can get the texture back
				m_editorID = heap->GetCurrentHeapIndex();
				device->CreateShaderResourceView(m_textureResource, &m_textureSRVDesc, heap->handleCPU(m_editorID));
				heap->IncrementHeapIndex();

				DirectX12Context.ReserveDescriptorHandle(m_textureResource, BindlessHeapRegion::TEXTURE2D, m_textureSRVDesc, m_rendererID);
			}
		}
		else if (desc.textureType == TextureType::CUBEMAP)
		{
			ID3D12Device* device = DirectX12Context.GetDevice();
			DirectX12CommandBuffer* copyCommandBuffer = DirectX12Context.GetCopyCommandBuffer();

			D3D12_RESOURCE_DESC textureDesc = {};
			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			textureDesc.MipLevels = 1;
			textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			textureDesc.DepthOrArraySize = 6;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;

			m_cubemapFilenames.push_back("assets/textures/Skybox/right.png");
			m_cubemapFilenames.push_back("assets/textures/Skybox/left.png");
			m_cubemapFilenames.push_back("assets/textures/Skybox/top.png");
			m_cubemapFilenames.push_back("assets/textures/Skybox/bottom.png");
			m_cubemapFilenames.push_back("assets/textures/Skybox/back.png");
			m_cubemapFilenames.push_back("assets/textures/Skybox/front.png");
			//Add to the subresource data struct for each image
			std::vector<D3D12_SUBRESOURCE_DATA> subresources;
			subresources.reserve(textureDesc.DepthOrArraySize);
			//Create 6 image files for the cubemap
			for (int i = 0; i < m_cubemapFilenames.size(); ++i)
			{
				int width, height, channels;
				stbi_uc* data = nullptr;
				{
					data = stbi_load(m_cubemapFilenames[i].c_str(), &width, &height, &channels, 0);
				}
				assert(data, "Failed to load image!");

				//We need to source the data for the 6 cubemap images somehow
				D3D12_SUBRESOURCE_DATA subresource_data{};
				subresource_data.pData = &data[0];
				subresource_data.RowPitch = width * channels;
				subresource_data.SlicePitch = subresource_data.RowPitch * height;
				subresources.push_back(subresource_data);
			}
			textureDesc.Width = 1024;
			textureDesc.Height = 1024;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			auto heapDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			HRESULT hr = device->CreateCommittedResource(&heapDefault, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_textureResource));
			if (FAILED(hr))
			{
				assert(0);
			}

			ID3D12Resource* bufferUploadHeap;
			UINT64 textureUploadBufferSize;
			device->GetCopyableFootprints(&textureDesc, 0, textureDesc.DepthOrArraySize, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);
			auto heapPropertiesUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize);
			hr = device->CreateCommittedResource(&heapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferUploadHeap));
			if (FAILED(hr))
			{
				assert(0);
			}

			bufferUploadHeap->SetName(L"Cubemap Texture upload resource");
			m_textureResource->SetName(L"Cubemap Texture final resource");
			UpdateSubresources(copyCommandBuffer->Get(), m_textureResource, bufferUploadHeap, 0, 0, static_cast<UINT>(subresources.size()), subresources.data());


			auto cResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_textureResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			copyCommandBuffer->Get()->ResourceBarrier(1, &cResourceBarrier);

			//Populate the SRV descriptor
			m_textureSRVDesc = {};
			ZeroMemory(&m_textureSRVDesc, sizeof(m_textureSRVDesc));
			m_textureSRVDesc.Texture2D.MipLevels = 1;
			m_textureSRVDesc.Texture2D.MostDetailedMip = 0;
			m_textureSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
			m_textureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			m_textureSRVDesc.TextureCube.MostDetailedMip = 0;
			m_textureSRVDesc.TextureCube.MipLevels = 1;
			m_textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			DirectX12Context.ReserveDescriptorHandle(m_textureResource, BindlessHeapRegion::CUBEMAPS, m_textureSRVDesc, m_rendererID);

			copyCommandBuffer->AddToCommandCallbackList([bufferUploadHeap, this]() {DirectX12Texture2D::Destroy(bufferUploadHeap); });
		}
	}

	void DirectX12Texture2D::Upload()
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());
		DirectX12CommandBuffer* copyCommandBuffer = DirectX12Context.GetCopyCommandBuffer();

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = m_width;
		resourceDesc.Height = m_height;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		auto cHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		HRESULT hr = device->CreateCommittedResource(&cHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_textureResource));
		if (FAILED(hr))
		{
			assert(0);
		}
		cHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);


		ID3D12Resource* bufferUploadHeap;
		UINT64 textureUploadBufferSize;
		device->GetCopyableFootprints(&resourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);
		auto cResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize);
		hr = device->CreateCommittedResource(&cHeapProperties, D3D12_HEAP_FLAG_NONE, &cResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferUploadHeap));
		if (FAILED(hr))
		{
			assert(0);
		}

		wchar_t* wString = new wchar_t[4096];
		MultiByteToWideChar(CP_ACP, 0, m_path.c_str(), -1, wString, 4096);
		bufferUploadHeap->SetName(wString);
		m_textureResource->SetName(wString);

		int bytesPerRow = 4 * m_width;
		m_textureData = {};
		m_textureData.pData = m_data;
		m_textureData.RowPitch = bytesPerRow;
		m_textureData.SlicePitch = bytesPerRow * m_height;

		copyCommandBuffer->UpdateCopyResource(m_textureResource, bufferUploadHeap, m_textureData);
		auto cResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_textureResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		copyCommandBuffer->Get()->ResourceBarrier(1, &cResourceBarrier);

		//Populate the SRV descriptor
		m_textureSRVDesc = {};
		ZeroMemory(&m_textureSRVDesc, sizeof(m_textureSRVDesc));
		m_textureSRVDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
		m_textureSRVDesc.Texture2D.MostDetailedMip = 0;
		m_textureSRVDesc.Format = resourceDesc.Format;
		m_textureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		m_textureSRVDesc.TextureCube.MostDetailedMip = 0;
		m_textureSRVDesc.TextureCube.MipLevels = 1;
		m_textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		//Populate the RTV descriptor
		m_textureRTVDesc = {};
		ZeroMemory(&m_textureRTVDesc, sizeof(m_textureRTVDesc));
		m_textureRTVDesc.Texture2D.MipSlice = 0;
		m_textureRTVDesc.Texture2D.PlaneSlice = 0;
		m_textureRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		m_textureRTVDesc.Format = resourceDesc.Format;

		//Place it into an ImGui Heap for now until I discuss an alternative
		DirectX12Heap* heap = DirectX12Context.GetImGuiHeap();
		//Create a shader resource and place it at the top of the index and increment the heap index, 
		//return the renderer ID for the heap so we can get the texture back
		m_editorID = heap->GetCurrentHeapIndex();
		device->CreateShaderResourceView(m_textureResource, &m_textureSRVDesc, heap->handleCPU(m_editorID));
		heap->IncrementHeapIndex();

		copyCommandBuffer->AddToCommandCallbackList([bufferUploadHeap, this]() { DirectX12Texture2D::Destroy(bufferUploadHeap); });

		DirectX12Context.ReserveDescriptorHandle(m_textureResource, BindlessHeapRegion::TEXTURE2D, m_textureSRVDesc, m_rendererID);
	}

	DirectX12Texture2D::~DirectX12Texture2D()
	{
		m_textureResource->Release();
		m_textureResource = 0;
	}

	uint64_t DirectX12Texture2D::GetHandle() const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = DirectX12Context.GetImGuiHeap()->handleGPU(GetRenderId());
		return textureHandle.ptr;
	}

	void DirectX12Texture2D::ResizeResource(const FramebufferTextureSpecification& framebufferTextureSpec, const FramebufferSpecification& framebufferSpec)
	{
		//Release the original resource
		m_textureResource->Release();
		m_textureResource = 0;

		m_width = framebufferSpec.Width;
		m_height = framebufferSpec.Height;

		assert(m_width > 0);
		assert(m_height > 0);

		//Create a default resource description for the texture
		//Liam fix - Lets deal with resource descriptions later
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());

		//we need to handle depth texture buffers slightly differently
		if (Util::IsDepthFormat(framebufferTextureSpec.textureFormat))
		{
			m_textureDSVDesc = {};
			m_textureDSVDesc.Format = Util::GetDXGIFormatType(framebufferTextureSpec.textureFormat);
			m_textureDSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			m_textureDSVDesc.Flags = D3D12_DSV_FLAG_NONE;

			D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
			depthOptimizedClearValue.Format = Util::GetDXGIFormatType(framebufferTextureSpec.textureFormat);
			depthOptimizedClearValue.DepthStencil.Depth = 0.0f;
			depthOptimizedClearValue.DepthStencil.Stencil = 0xff;
			auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			auto textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, framebufferSpec.Width, framebufferSpec.Height, 1, 0, framebufferSpec.Samples, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
			HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &depthOptimizedClearValue, IID_PPV_ARGS(&m_textureResource));
			if (FAILED(hr))
			{
				assert(0);
			}
			m_textureResource->SetName(L"Depth24Stencil8 Framebuffer");
			//device->CreateDepthStencilView(m_textureResource, &resourceDesc, )
			m_textureSRVDesc = {};
			m_textureSRVDesc.Format = Util::GetDXGIFormatType(framebufferTextureSpec.textureFormat);
			m_textureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			m_textureSRVDesc.Texture2D.MipLevels = 1;
			m_textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			DirectX12Context.ReserveDescriptorHandle(m_textureResource, BindlessHeapRegion::TEXTURE2D, m_textureSRVDesc, m_rendererID);

		}
		//Standard RTVC 
		else
		{
			D3D12_RESOURCE_DESC resourceDesc = {};
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			resourceDesc.Alignment = 0;
			resourceDesc.Width = framebufferSpec.Width;
			resourceDesc.Height = framebufferSpec.Height;
			resourceDesc.DepthOrArraySize = 1;
			resourceDesc.MipLevels = 1;
			resourceDesc.Format = Util::GetDXGIFormatType(framebufferTextureSpec.textureFormat);
			resourceDesc.SampleDesc.Count = framebufferSpec.Samples;
			resourceDesc.SampleDesc.Quality = 0;
			resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

			D3D12_CLEAR_VALUE clearValue = {};
			clearValue.Color[0] = 0.0f;
			clearValue.Color[1] = 0.0f;
			clearValue.Color[2] = 0.0f;
			clearValue.Color[3] = 1.0f;
			clearValue.Format = resourceDesc.Format;
			auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearValue, IID_PPV_ARGS(&m_textureResource));
			if (FAILED(hr))
			{
				assert(0);
			}

			m_textureResource->SetName(L"Framebuffer Texture");

			////Populate the SRV descriptor
			m_textureSRVDesc = {};
			ZeroMemory(&m_textureSRVDesc, sizeof(m_textureSRVDesc));
			m_textureSRVDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
			m_textureSRVDesc.Texture2D.MostDetailedMip = 0;
			m_textureSRVDesc.Format = resourceDesc.Format;
			m_textureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			m_textureSRVDesc.TextureCube.MostDetailedMip = 0;
			m_textureSRVDesc.TextureCube.MipLevels = 1;
			m_textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


			//Populate the RTV descriptor
			m_textureRTVDesc = {};
			ZeroMemory(&m_textureRTVDesc, sizeof(m_textureRTVDesc));
			m_textureRTVDesc.Texture2D.MipSlice = 0;
			m_textureRTVDesc.Texture2D.PlaneSlice = 0;
			m_textureRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			m_textureRTVDesc.Format = resourceDesc.Format;


			m_textureUAVDesc = {};
			ZeroMemory(&m_textureUAVDesc, sizeof(m_textureUAVDesc));
			m_textureUAVDesc.Format = resourceDesc.Format;
			m_textureUAVDesc.Texture2D.MipSlice = 0;
			m_textureUAVDesc.Texture2D.PlaneSlice = 0;
			m_textureUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;


			////Place it into an ImGui Heap for now until I discuss an alternative
			DirectX12Heap* heap = DirectX12Context.GetImGuiHeap();
			//Create a shader resource (FOR IMGUI) and place it at the top of the index and increment the heap index, 
			//return the renderer ID for the heap so we can get the texture back
			m_editorID = heap->GetCurrentHeapIndex();
			device->CreateShaderResourceView(m_textureResource, &m_textureSRVDesc, heap->handleCPU(m_editorID));
			heap->IncrementHeapIndex();

			DirectX12Context.ReserveDescriptorHandle(m_textureResource, BindlessHeapRegion::TEXTURE2D, m_textureSRVDesc, m_rendererID);
		}


	}

	void DirectX12Texture2D::SetData(void* data, uint32_t size)
	{

	}

	void DirectX12Texture2D::Bind(uint32_t slot) const
	{

	}

	void DirectX12Texture2D::Unbind(uint32_t slot) const
	{

	}

	void DirectX12Texture2D::CreateRTV(DirectX12Heap* inRTVHeap)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());
		m_framebufferRTVHeapIndex = inRTVHeap->GetCurrentHeapIndex();
		device->CreateRenderTargetView(m_textureResource, &m_textureRTVDesc, inRTVHeap->handleCPU(m_framebufferRTVHeapIndex));
		inRTVHeap->IncrementHeapIndex(); //Increment the heap index
	}

	void DirectX12Texture2D::RecreateRTV(DirectX12Heap* inRTVHeap)
	{
		//Recreate the RTV at the same location we had before using the stored m_framebufferRTVIndex
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());
		device->CreateRenderTargetView(m_textureResource, &m_textureRTVDesc, inRTVHeap->handleCPU(m_framebufferRTVHeapIndex));
	}

	void DirectX12Texture2D::CreateDSV(DirectX12Heap* inDSVHeap)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());
		m_framebufferDSVHeapIndex = inDSVHeap->GetCurrentHeapIndex();
		device->CreateDepthStencilView(m_textureResource, &m_textureDSVDesc, inDSVHeap->handleCPU(m_framebufferDSVHeapIndex));
		inDSVHeap->IncrementHeapIndex();//Increment the heap index
	}

	void DirectX12Texture2D::RecreateDSV(DirectX12Heap* inDSVHeap)
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());
		device->CreateDepthStencilView(m_textureResource, &m_textureDSVDesc, inDSVHeap->handleCPU(m_framebufferDSVHeapIndex));
	}
}