#include "directx12_texture.h"
#include "directx12_graphics_context.h"
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
		m_Width = desc.width;
		m_Height = desc.height;
		m_BitDepth = desc.bitDepth;
		m_MipCount = desc.mipCount;
		m_TextureType = desc.textureType;
		m_TextureFormat = desc.textureFormat;
		m_TextureUsage = desc.textureUsage;

		//Fast assert
		assert(m_Width > 0);
		assert(m_Height > 0);

		if (m_TextureType & TextureType::Asset)
		{
			//Fast assert for invalid intial data from asset file
			if (!desc.initialData)
				assert(0);
			

			m_Data = desc.initialData;
			//TODO:
			//Delayed upload/resource managemment

		}

		if (m_TextureType & TextureType::Framebuffer)
		{
			//Create a default resource description for the texture
			//Liam fix - Lets deal with resource descriptions later
			ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());

			//we need to handle depth texture buffers slightly differently
			if (Util::IsDepthFormat(m_TextureFormat))
			{
				m_textureDSVDesc = {};
				m_textureDSVDesc.Format = Util::GetDXGIFormatType(m_TextureFormat);
				m_textureDSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				m_textureDSVDesc.Flags = D3D12_DSV_FLAG_NONE;

				D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
				depthOptimizedClearValue.Format = Util::GetDXGIFormatType(m_TextureFormat);
				depthOptimizedClearValue.DepthStencil.Depth = 0.0f;
				depthOptimizedClearValue.DepthStencil.Stencil = 0xff;

				auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				auto textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, m_Width, m_Height, 1, 0, m_Samples, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
				HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &depthOptimizedClearValue, IID_PPV_ARGS(&m_textureResource));
				if (FAILED(hr))
				{
					assert(0);
				}
				m_textureResource->SetName(desc.debugName);
				
				m_textureSRVDesc = {};
				m_textureSRVDesc.Format = Util::GetDXGIFormatType(m_TextureFormat);
				m_textureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				m_textureSRVDesc.Texture2D.MipLevels = 1;
				m_textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				m_srvDescriptorHandleBlock = DirectX12Context.ReserveDescriptorHandle(ReservedHeapRegion::BindlessTexture2D);
				m_dsvDescriptorHandleBlock = DirectX12Context.ReserveDescriptorHandle(ReservedHeapRegion::DepthStencil);
				DirectX12Context.CreateSRV(m_textureResource, m_srvDescriptorHandleBlock, m_textureSRVDesc);
				DirectX12Context.CreateDSV(m_textureResource, m_dsvDescriptorHandleBlock, m_textureDSVDesc);
			}
			//Standard RTVC 
			else
			{
				D3D12_RESOURCE_DESC resourceDesc = {};
				resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				resourceDesc.Alignment = 0;
				resourceDesc.Width = m_Width;
				resourceDesc.Height = m_Height;
				resourceDesc.DepthOrArraySize = 1;
				resourceDesc.MipLevels = m_MipCount;
				resourceDesc.Format = Util::GetDXGIFormatType(m_TextureFormat);
				resourceDesc.SampleDesc.Count = m_Samples;
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
				m_textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				//Populate the RTV descriptor
				m_textureRTVDesc = {};
				ZeroMemory(&m_textureRTVDesc, sizeof(m_textureRTVDesc));
				m_textureRTVDesc.Format = resourceDesc.Format;
				m_textureRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				m_textureRTVDesc.Texture2D.MipSlice = 0;
				m_textureRTVDesc.Texture2D.PlaneSlice = 0;
				
				m_srvDescriptorHandleBlock = DirectX12Context.ReserveDescriptorHandle(ReservedHeapRegion::BindlessTexture2D);
				DirectX12Context.CreateSRV(m_textureResource, m_srvDescriptorHandleBlock, m_textureSRVDesc);
			}
		}
	}

	//Don't like this. Need to remove
	void DirectX12Texture2D::Upload()
	{
		ID3D12Device* device = static_cast<ID3D12Device*>(DirectX12Context.GetDevice());
		DirectX12CommandBuffer* copyCommandBuffer = DirectX12Context.GetCopyCommandBuffer();

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = m_Width;
		resourceDesc.Height = m_Height;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = m_MipCount;
		resourceDesc.Format = Util::GetDXGIFormatType(m_TextureFormat);
		resourceDesc.SampleDesc.Count = m_Samples;
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
		MultiByteToWideChar(CP_ACP, 0, "", -1, wString, 4096);
		bufferUploadHeap->SetName(wString);
		m_textureResource->SetName(wString);

		int bytesPerRow = 4 * m_Width;
		m_textureData = {};
		m_textureData.pData = m_Data;
		m_textureData.RowPitch = bytesPerRow;
		m_textureData.SlicePitch = bytesPerRow * m_Height;

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

		copyCommandBuffer->AddToCommandCallbackList([bufferUploadHeap, this]() { DirectX12Texture2D::Destroy(bufferUploadHeap); });

		m_srvDescriptorHandleBlock = DirectX12Context.ReserveDescriptorHandle(ReservedHeapRegion::BindlessTexture2D);
		m_rtvDescriptorHandleBlock = DirectX12Context.ReserveDescriptorHandle(ReservedHeapRegion::RenderTarget);
		DirectX12Context.CreateSRV(m_textureResource, m_srvDescriptorHandleBlock, m_textureSRVDesc);
		DirectX12Context.CreateRTV(m_textureResource, m_rtvDescriptorHandleBlock, m_textureRTVDesc);
	}

	DirectX12Texture2D::~DirectX12Texture2D()
	{
		m_textureResource->Release();
		m_textureResource = 0;
	}
}