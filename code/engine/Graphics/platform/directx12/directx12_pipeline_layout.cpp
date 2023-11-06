#include "directx12_pipeline_layout.h"
#include "directx12_graphics_context.h"
#include <d3dcompiler.h>
#include "d3dx12.h"
#include <vector>
#include <utility>
namespace KNR
{
	constexpr D3D12_SHADER_VISIBILITY _ConvertShaderVisibility(ShaderVisibility visibility)
	{
		switch (visibility)
		{
		case ShaderVisibility::all:
			return D3D12_SHADER_VISIBILITY_ALL;
		case ShaderVisibility::vertex:
			return D3D12_SHADER_VISIBILITY_VERTEX;
		case ShaderVisibility::pixel:
			return D3D12_SHADER_VISIBILITY_PIXEL;
		case ShaderVisibility::hull:
			return D3D12_SHADER_VISIBILITY_HULL;
		case ShaderVisibility::geometry:
			return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case ShaderVisibility::mesh:
			return D3D12_SHADER_VISIBILITY_MESH;
		case ShaderVisibility::domain:
			return D3D12_SHADER_VISIBILITY_DOMAIN;
		default:
			return D3D12_SHADER_VISIBILITY_ALL;
		}

		//We shouldn't get here, we've fucked up somewhere
		assert(0);
	}

	constexpr D3D12_DESCRIPTOR_RANGE_TYPE _ConvertDescriptorRangeType(DescriptorTableType tableType)
	{
		switch (tableType)
		{
		case DescriptorTableType::cbv:
			return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case DescriptorTableType::srv:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case DescriptorTableType::uav:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		}

		//We shouldn't get here, we've fucked up somewhere
		assert(0);
	}

	constexpr D3D12_FILTER _ConvertSamplerType(SamplerType samplerType)
	{
		switch (samplerType)
		{
		case SamplerType::point:
			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		case SamplerType::min_mag_mip_linear:
			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		case SamplerType::anisotropic:
			return D3D12_FILTER_ANISOTROPIC;
		}

		//We shouldn't get here, we've fucked up somewhere
		assert(0);
	}

	DirectX12PipelineLayout::DirectX12PipelineLayout()
	{

	}

	DirectX12PipelineLayout::~DirectX12PipelineLayout()
	{
		//m_rootSignature->Release();
		//m_rootSignature = 0;
	}

	void DirectX12PipelineLayout::AddDescriptorTable(const std::string& name, const uint32_t registerSlot, const int32_t numDescriptors, const uint32_t registerSpace, const DescriptorTableType tableType, const ShaderVisibility visibility)
	{
		CD3DX12_DESCRIPTOR_RANGE range = {};
		range.Init(_ConvertDescriptorRangeType(tableType), numDescriptors, registerSlot, registerSpace);

		m_descriptorRangeList.push_back(std::move(range));

		CD3DX12_ROOT_PARAMETER rootParameter = {};
		rootParameter.InitAsDescriptorTable(1, &m_descriptorRangeList.back(), _ConvertShaderVisibility(visibility));

		m_rootParameters.emplace_back(std::move(rootParameter));

		m_paramMap.emplace(std::pair(name, m_paramMap.size()));
	}

	void DirectX12PipelineLayout::AddRootConstant(const std::string& name, const uint32_t numOfValues, const uint32_t registerSlot, const uint32_t registerSpace, const ShaderVisibility visibility)
	{
		CD3DX12_ROOT_PARAMETER rootParameter = {};
		rootParameter.InitAsConstants(numOfValues, registerSlot, registerSpace, _ConvertShaderVisibility(visibility));

		m_rootParameters.push_back(std::move(rootParameter));

		m_paramMap.emplace(std::pair(name, m_paramMap.size()));
	}

	void DirectX12PipelineLayout::AddShaderResourceView(const std::string& name, const uint32_t registerSlot, const ShaderVisibility visibility)
	{
		CD3DX12_ROOT_PARAMETER rootParameter = {};
		rootParameter.InitAsShaderResourceView(registerSlot, 0, _ConvertShaderVisibility(visibility));

		m_rootParameters.push_back(std::move(rootParameter));

		m_paramMap.emplace(std::pair(name, m_paramMap.size()));
	}

	void DirectX12PipelineLayout::AddConstantResourceView(const std::string& name, const uint32_t registerSlot, const ShaderVisibility visibility)
	{
		CD3DX12_ROOT_PARAMETER rootParameter = {};
		rootParameter.InitAsConstantBufferView(registerSlot, 0, _ConvertShaderVisibility(visibility));

		m_rootParameters.push_back(std::move(rootParameter));

		m_paramMap.emplace(std::pair(name, m_paramMap.size()));
	}

	void DirectX12PipelineLayout::AddUnorderedAccessView(const std::string& name, const uint32_t registerSlot, const ShaderVisibility visibility)
	{
		CD3DX12_ROOT_PARAMETER rootParameter = {};
		rootParameter.InitAsUnorderedAccessView(registerSlot, 0, _ConvertShaderVisibility(visibility));

		m_rootParameters.push_back(std::move(rootParameter));

		m_paramMap.emplace(std::pair(name, m_paramMap.size()));
	}

	void DirectX12PipelineLayout::AddSampler(const std::string& name, const uint32_t registerSlot, const SamplerType samplerType)
	{
		//We only handle simple linear point samplers for now 
		CD3DX12_STATIC_SAMPLER_DESC staticSampler = {};
		staticSampler.Init(registerSlot, _ConvertSamplerType(samplerType));

		m_staticSamplers.push_back(staticSampler);
	}

	void DirectX12PipelineLayout::CreateRootSignature()
	{
		ID3D12Device* device = DirectX12Context.GetDevice();
		HRESULT result;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(m_rootParameters.size(), m_rootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
		rootSignatureDesc.NumStaticSamplers = m_staticSamplers.size();
		rootSignatureDesc.pStaticSamplers = m_staticSamplers.data();
		
		ID3DBlob* signature;
		result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
		if (FAILED(result))
		{
			assert(0);
		}

		result = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_rootSignature);
		if (FAILED(result))
		{
			assert(0);
		}
	}
}