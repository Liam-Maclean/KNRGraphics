#pragma once
#include "directx12_graphics_context.h"
#include "texture.h"
#include <string>
#include <d3d12.h>

namespace KNR
{
	class DirectX12Heap;
	class DirectX12Texture2D final : public Texture2D
	{
	public:
		DirectX12Texture2D(const TextureDescriptor& desc);
		virtual ~DirectX12Texture2D();

		virtual void Upload() override;
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetBindlessIndex() const override { return m_srvDescriptorHandleBlock.bindlessIndexID; }
		virtual uint64_t GetHandle() const override;
		
		inline void Destroy(ID3D12Resource* uploadBuffer, void* data) 
		{
			uploadBuffer->Release(); 
			uploadBuffer = 0; 
			if (data != nullptr) { delete data; data = nullptr; 
			}
		};
		inline void Destroy(ID3D12Resource* uploadBuffer) 
		{
			uploadBuffer->Release(); 
			uploadBuffer = 0;

			//TODO
			//SAFE_DELETE(m_data);
		};

		ID3D12Resource* GetTextureHandle() { return m_textureResource; }

		virtual bool operator ==(const Texture& other) const override
		{
			return m_srvDescriptorHandleBlock.bindlessIndexID == other.GetBindlessIndex();
		}
	private:

		//Resource
		ID3D12Resource* m_textureResource;
		
		//Resource views
		D3D12_SHADER_RESOURCE_VIEW_DESC m_textureSRVDesc;
		D3D12_RENDER_TARGET_VIEW_DESC m_textureRTVDesc;
		D3D12_DEPTH_STENCIL_VIEW_DESC m_textureDSVDesc;
		D3D12_UNORDERED_ACCESS_VIEW_DESC m_textureUAVDesc;
		
		//Descriptor handles
		DirectX12DescriptorHandleBlock m_srvDescriptorHandleBlock;
		DirectX12DescriptorHandleBlock m_rtvDescriptorHandleBlock;
		DirectX12DescriptorHandleBlock m_dsvDescriptorHandleBlock;

		//Other
		std::vector<std::string> m_cubemapFilenames;
		D3D12_SUBRESOURCE_DATA m_textureData = {};
	};
}