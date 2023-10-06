#include <qlpch.h>
#include "material.h"
#include "graphics/platform/directx/directx_command_buffer.h"

namespace QRender
{
	static PBRMaterialShaderData defaultMaterial =
	{
		.baseColorFactor = glm::vec4(1.0f,0.0f, 0.0f, 1.0f),
		.emissiveFactor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		.diffuseFactor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		.specularFactor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
		.albedoIdx = -1, //no textures (-1)
		.normalIdx = -1,
		.metallicIdx = -1,
		.occlusionIdx = -1,
		.emissiveIdx = -1,
		.metallicFactor = 0.5f,
		.roughnessFactor = 1.0f,
		.alphaMask = 0.0f,
		.alphaMaskCutoff = 0.0f
	};

	static PBRMaterialShaderData defaultBlackMaterial =
	{
		.baseColorFactor = glm::vec4(0.0f,0.0f, 0.0f, 1.0f),
		.emissiveFactor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		.diffuseFactor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		.specularFactor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		.albedoIdx = -1, //no textures (-1)
		.normalIdx = -1,
		.metallicIdx = -1,
		.occlusionIdx = -1,
		.emissiveIdx = -1,
		.metallicFactor = 0.0f,
		.roughnessFactor = 1.0f,
		.alphaMask = 0.0f,
		.alphaMaskCutoff = 0.0f
	};

	Material::Material(const Archive& archive)
		: Archive(archive)
	{
		Deserialize();
	}

	void Material::Deserialize()
	{
		m_materialType = GetValue<MaterialType>(ArchiveKeys::Material::MaterialType);
	}


	PBRMaterial::PBRMaterial()
		: Material()
	{
		SetMaterialShaderData(defaultBlackMaterial);
	}

	PBRMaterial::PBRMaterial(const Archive& archive)
	{
		Deserialize();
	}

	PBRMaterial::~PBRMaterial()
	{
		
	}

	void PBRMaterial::CreateShaderResources()
	{
		//m_materialUniformBuffer = QRender::UniformBuffer::Create(sizeof(PBRMaterialShaderData));
		//m_materialUniformBuffer->SetData(&m_materialShaderData, sizeof(PBRMaterialShaderData));
	}

	void PBRMaterial::Bind(Ref<DirectXCommandBuffer> commandList, uint32_t slot)
	{
		//m_materialUniformBuffer->Bind(commandList, slot);
	}

	void PBRMaterial::Unbind()
	{

	}

	void PBRMaterial::Serialize()
	{
		SetValue(ArchiveKeys::PBRMaterial::AlbedoTexture, m_albedoTexture);
		SetValue(ArchiveKeys::PBRMaterial::NormalTexture, m_normalTexture);
		SetValue(ArchiveKeys::PBRMaterial::MetallicTexture, m_metallicTexture);
		SetValue(ArchiveKeys::PBRMaterial::OcclusionTexture, m_aoTexture);
		SetValue(ArchiveKeys::PBRMaterial::EmissiveTexture, m_emissiveTexture);

		SetValue(ArchiveKeys::PBRMaterial::BaseColorFactor, m_materialShaderData.baseColorFactor);
		SetValue(ArchiveKeys::PBRMaterial::EmissiveFactor, m_materialShaderData.emissiveFactor);
		SetValue(ArchiveKeys::PBRMaterial::DiffuseFactor, m_materialShaderData.diffuseFactor);
		SetValue(ArchiveKeys::PBRMaterial::SpecularFactor, m_materialShaderData.specularFactor);
		SetValue(ArchiveKeys::PBRMaterial::MetallicFactor, m_materialShaderData.metallicFactor);
		SetValue(ArchiveKeys::PBRMaterial::RoughnessFactor, m_materialShaderData.roughnessFactor);
		SetValue(ArchiveKeys::PBRMaterial::AlphaMask, m_materialShaderData.alphaMask);
		SetValue(ArchiveKeys::PBRMaterial::AlphaMaskCutoff, m_materialShaderData.alphaMaskCutoff);
	}

	void PBRMaterial::Deserialize()
	{
		m_albedoTexture = GetValue<AssetMetadata>(ArchiveKeys::PBRMaterial::AlbedoTexture);
		m_normalTexture = GetValue<AssetMetadata>(ArchiveKeys::PBRMaterial::NormalTexture);
		m_metallicTexture = GetValue<AssetMetadata>(ArchiveKeys::PBRMaterial::MetallicTexture);
		m_aoTexture = GetValue<AssetMetadata>(ArchiveKeys::PBRMaterial::OcclusionTexture);
		m_emissiveTexture = GetValue<AssetMetadata>(ArchiveKeys::PBRMaterial::EmissiveTexture);

		m_materialShaderData.baseColorFactor =	GetValue<Vector4f>(ArchiveKeys::PBRMaterial::BaseColorFactor);
		m_materialShaderData.emissiveFactor = GetValue<Vector4f>(ArchiveKeys::PBRMaterial::EmissiveFactor);
		m_materialShaderData.diffuseFactor = GetValue<Vector4f>(ArchiveKeys::PBRMaterial::DiffuseFactor);
		m_materialShaderData.specularFactor = GetValue<Vector4f>(ArchiveKeys::PBRMaterial::SpecularFactor);
		m_materialShaderData.metallicFactor = GetValue<float>(ArchiveKeys::PBRMaterial::MetallicFactor);
		m_materialShaderData.roughnessFactor = GetValue<float>(ArchiveKeys::PBRMaterial::RoughnessFactor);
		m_materialShaderData.alphaMask = GetValue<float>(ArchiveKeys::PBRMaterial::AlphaMask);
		m_materialShaderData.alphaMaskCutoff = GetValue<float>(ArchiveKeys::PBRMaterial::AlphaMaskCutoff);
	}
	
}
