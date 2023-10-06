#pragma once
#include "shader.h"
#include "logging/log.h"
#include "serialization/archive.h"

//Liam: Material classes to be serialized to .mtl files
// 
//Any new materials should be inherited from Material and have their own shader data associated with the 
//structure of the material used in the shader
//
//For now we're only using PBR materials which should cover everything rendering opaque 
//
//TO:DO Transparent material

namespace ArchiveKeys
{
	namespace Material
	{
		const ArchiveEntry MaterialType = ArchiveEntry("/MaterialType");
	}

	namespace PBRMaterial
	{
		const ArchiveEntry AlbedoTexture = ArchiveEntry("/AlbedoTexture");
		const ArchiveEntry NormalTexture = ArchiveEntry("/NormalTexture");
		const ArchiveEntry MetallicTexture = ArchiveEntry("/MetallicTexture");
		const ArchiveEntry OcclusionTexture = ArchiveEntry("/OcclusionTexture");
		const ArchiveEntry EmissiveTexture = ArchiveEntry("/EmmisiveTexture");

		const ArchiveEntry BaseColorFactor = ArchiveEntry("/BaseColorFactor");
		const ArchiveEntry EmissiveFactor = ArchiveEntry("/EmissiveFactor");
		const ArchiveEntry DiffuseFactor = ArchiveEntry("/DiffuseFactor");
		const ArchiveEntry SpecularFactor = ArchiveEntry("/SpecularFactor");

		const ArchiveEntry MetallicFactor = ArchiveEntry("/MetallicFactor");
		const ArchiveEntry RoughnessFactor = ArchiveEntry("/RoughnessFactor");
		const ArchiveEntry AlphaMask = ArchiveEntry("/AlphaMask");
		const ArchiveEntry AlphaMaskCutoff = ArchiveEntry("/AlphaMaskCutoff");
	}
}


class Archive;
namespace QRender
{
	class DirectXCommandBuffer;
	class Buffer;
	enum class MaterialType
	{
		None,
		Pbr,
		Transparent
	};

	struct PBRMaterialShaderData
	{
		glm::vec4 baseColorFactor;
		glm::vec4 emissiveFactor;
		glm::vec4 diffuseFactor;
		glm::vec4 specularFactor;
		int albedoIdx = 0;
		int normalIdx = 0;
		int metallicIdx = 0;
		int occlusionIdx = 0;
		int emissiveIdx = 0;
		float metallicFactor;
		float roughnessFactor;
		float alphaMask;
		float alphaMaskCutoff;
	};

	class Material : public Archive
	{
	public:
		Material() { m_materialType = MaterialType::None; }; //Default constructor
		Material(Material&) = default;
		Material(Material&&) = default;
		Material(const Material&) = default;
		Material(const Archive& archive);
		virtual ~Material() {};
		virtual void CreateShaderResources() = 0;

		virtual void Bind(Ref<DirectXCommandBuffer> commandList, uint32_t slot) = 0;
		virtual void Unbind() = 0;

		virtual void Serialize() = 0;
		virtual void Deserialize();

		Material& operator=(const Material&) = default;
		MaterialType GetMaterialType() { return m_materialType; }
	private:
		MaterialType m_materialType;
	};


	class PBRMaterial : public Material
	{
	public:
		PBRMaterial(); // Default constructor
		PBRMaterial(PBRMaterial&) = default;
		PBRMaterial(PBRMaterial&&) = default;
		PBRMaterial(const PBRMaterial&) = default;
		PBRMaterial(const Archive& archive);
		virtual ~PBRMaterial();

		//Setters
		inline void SetMaterialShaderData(PBRMaterialShaderData materialShaderData) { m_materialShaderData = materialShaderData; }

		inline void SetBaseColorFactor(const glm::vec4& color) { m_materialShaderData.baseColorFactor = color; }
		inline void SetEmissiveColorFactor(const glm::vec4& color) { m_materialShaderData.emissiveFactor = color; }
		inline void SetDiffuseColorFactor(const glm::vec4& color) { m_materialShaderData.diffuseFactor = color; }
		inline void SetSpecularColorFactor(const glm::vec4& color) { m_materialShaderData.specularFactor = color; }
		inline void SetMetallicFactor(float metallicFactor) { m_materialShaderData.metallicFactor = metallicFactor; }
		inline void SetEmissiveFactor(float roughnessFactor) { m_materialShaderData.roughnessFactor = roughnessFactor; }
		inline void SetAlphaMask(float alphaMask) { m_materialShaderData.alphaMask = alphaMask; }
		inline void SetAlphaMaskCutoff(float alphaMaskCutoff) { m_materialShaderData.alphaMaskCutoff = alphaMaskCutoff; }

		inline void SetAlbedoTexture(const AssetMetadata& texture) { m_albedoTexture = texture; }
		inline void SetNormalTexture(const AssetMetadata& texture) { m_normalTexture = texture; }
		inline void SetMetallicTexture(const AssetMetadata& texture) { m_metallicTexture = texture; }
		inline void SetAOTexture(const AssetMetadata& texture) { m_aoTexture = texture; }
		inline void SetEmissiveTexture(const AssetMetadata& texture) { m_emissiveTexture = texture; }

		//Getters
		inline PBRMaterialShaderData GetMaterialShaderData() const { return m_materialShaderData; }

		inline Vector4f GetBaseColorFactor() { return m_materialShaderData.baseColorFactor; }
		inline Vector4f GetEmissiveColorFactor() { return m_materialShaderData.emissiveFactor; }
		inline Vector4f GetDiffuseColorFactor() { return m_materialShaderData.diffuseFactor; }
		inline Vector4f GetSpecularColorFactor() { return m_materialShaderData.specularFactor; }
		inline float GetMetallicFactor() { return m_materialShaderData.metallicFactor; }
		inline float GetEmissiveFactor() { return m_materialShaderData.roughnessFactor; }
		inline float GetAlphaMask() { return m_materialShaderData.alphaMask; }
		inline float GetAlphaMaskCutoff() { return m_materialShaderData.alphaMaskCutoff; }

		inline AssetMetadata GetAlbedoTexture() { return m_albedoTexture; }
		inline AssetMetadata GetNormalTexture() { return m_normalTexture; }
		inline AssetMetadata GetMetallicTexture() { return m_metallicTexture; }
		inline AssetMetadata GetAOTexture() { return m_aoTexture; }
		inline AssetMetadata GetEmissiveTexture() { return m_emissiveTexture; }

		//Functions
		virtual void CreateShaderResources() override;
		virtual void Bind(Ref<DirectXCommandBuffer> commandList, uint32_t slot) override;
		virtual void Unbind() override;
		virtual void Serialize() override;
		virtual void Deserialize() override;

		PBRMaterial& operator=(const PBRMaterial&) = default;
	private:
		PBRMaterialShaderData m_materialShaderData;
		Ref<Buffer> m_materialUniformBuffer;

		AssetMetadata m_albedoTexture;
		AssetMetadata m_normalTexture;
		AssetMetadata m_metallicTexture;
		AssetMetadata m_aoTexture;
		AssetMetadata m_emissiveTexture;
	};
}
