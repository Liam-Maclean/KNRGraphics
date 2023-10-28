#pragma once
#include "directx12_graphics_context.h"
#include "texture.h"
#include <string>
#include <d3d12.h>

namespace KNR
{
	class DirectX12Heap;
	class DirectX12Texture2D : public Texture2D
	{
	public:
		DirectX12Texture2D(const TextureDescriptor& desc);
		virtual ~DirectX12Texture2D();

		virtual void Upload() override;
		virtual uint32_t GetWidth() const override { return m_width; }
		virtual uint32_t GetHeight() const override { return m_height; }
		virtual uint32_t GetRenderId() const override { return m_rendererID; }
		virtual uint32_t GetEditorRenderId() const override { return m_editorID; }
		virtual uint64_t GetHandle() const override;
		
		virtual void ResizeResource(FramebufferTextureSpecification framebufferTextureSpec, FramebufferSpecification framebufferSpec);
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
			SAFE_DELETE(m_data);
		};

		virtual void SetData(void* data, uint32_t size) override;
		virtual void Bind(uint32_t slot = 0) const override;
		virtual void Unbind(uint32_t slot = 0) const override;

		ID3D12Resource* GetTextureHandle() { return m_textureResource; }
		void CreateRTV(DirectX12Heap* inRTVHeap);
		void RecreateRTV(DirectX12Heap* inRTVHeap);

		void CreateDSV(DirectX12Heap* inDSVHeap);
		void RecreateDSV(DirectX12Heap* inDSVHeap);

		virtual bool operator ==(const Texture& other) const override
		{
			return m_rendererID == other.GetRenderId();
		}
	private:
		ID3D12Resource* m_textureResource;
		D3D12_SHADER_RESOURCE_VIEW_DESC m_textureSRVDesc;
		D3D12_RENDER_TARGET_VIEW_DESC m_textureRTVDesc;
		D3D12_DEPTH_STENCIL_VIEW_DESC m_textureDSVDesc;
		D3D12_UNORDERED_ACCESS_VIEW_DESC m_textureUAVDesc;
		DirectX12DescriptorHandleBlock m_descriptorHandleBlock;
		std::vector<std::string> m_cubemapFilenames;

		std::string m_path = "";
		int m_width = 0;
		int m_height = 0;
		int m_channels = 0;
		void* m_data = nullptr;
		RENDERERID m_rendererID = -1;	//Shader
		RENDERERID m_editorID = -1;		//ImGui

		uint32_t m_framebufferRTVHeapIndex = -1;
		uint32_t m_framebufferDSVHeapIndex = -1;
		D3D12_SUBRESOURCE_DATA m_textureData = {};
	};
}