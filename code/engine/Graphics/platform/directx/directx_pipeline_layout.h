#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <vector>

namespace KNR
{
	namespace DirectXVertexInputLayouts
	{
		static const D3D12_INPUT_ELEMENT_DESC kVertexInputQuad[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		static const D3D12_INPUT_ELEMENT_DESC kVertexInputStaticMesh[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		static const D3D12_INPUT_ELEMENT_DESC kVertexInputSkinnedMesh[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"WORLD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};
	}

	enum class ResourceType
	{
		Texture,
		Sampler,
		Buffer
	};

	enum class DescriptorTableType
	{
		cbv,
		srv,
		uav
	};

	enum class ShaderVisibility
	{
		unknown,
		all,
		vertex,
		pixel,
		domain,
		hull,
		geometry,
		mesh
	};

	enum class SamplerType
	{
		point,
		min_mag_mip_linear,
		anisotropic,
	};

	class DirectXPipelineLayout
	{
	public:
		DirectXPipelineLayout();
		~DirectXPipelineLayout();

		void AddDescriptorTable(const std::string& name, uint32_t registerSlot, int32_t numDescriptors, uint32_t registerSpace, DescriptorTableType tableType, ShaderVisibility visibility);
		void AddRootConstant(const std::string& name, uint32_t numOfValues, uint32_t registerSlot, uint32_t registerSpace, ShaderVisibility visibility);
		void AddShaderResourceView(const std::string& name, uint32_t registerSlot, ShaderVisibility visibility);
		void AddConstantResourceView(const std::string& name, uint32_t registerSlot, ShaderVisibility visibility);
		void AddUnorderedAccessView(const std::string& name, uint32_t registerSlot, ShaderVisibility visibility);
		void AddSampler(const std::string& name, uint32_t registerSlot, SamplerType samplerType);
		void CreateRootSignature();
		ID3D12RootSignature* GetRootSignature() { return m_rootSignature; }

	private:
		//Register + root parameter so we can check if a root parameter already exists at that register slot
		std::vector<CD3DX12_ROOT_PARAMETER> m_rootParameters;
		std::vector<CD3DX12_ROOT_CONSTANTS> m_rootConstants;
		std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_staticSamplers;
		std::map<std::string, uint32_t> m_paramMap;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElementDesc;
		ID3D12RootSignature* m_rootSignature;
		std::vector<CD3DX12_DESCRIPTOR_RANGE> m_descriptorRanges;
		std::list< CD3DX12_DESCRIPTOR_RANGE> m_descriptorRangeList;
	};
}