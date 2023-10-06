#pragma once
#include "core/core.h"
#include "graphics/buffer.h"
#include "graphics/material.h"
namespace QRender
{
	struct GerstnerWave
	{
		glm::vec2 direction;
		float amplitude;
		float wavelength;
		float waveSpeed;
		glm::vec3 padding;
	};

	class Plane
	{
	public:
		Plane();
		~Plane();
		float GetRandomFloat(float randMin, float randMax);
		void CreatePlaneGrid();
		void Update();
		void Draw(Ref<DirectXCommandBuffer> commandList);
	protected:

	private:
		Ref <Buffer> m_vertexBuffer;
		Ref <Buffer> m_indexBuffer;

		float m_waveDirectionMin = 0.6f;
		float m_waveDirectionMax = 1.0f;
		float m_waveAmplitudeMin = 2.0f;
		float m_waveAmplitudeMax = 5.0f;
		float m_waveLengthMin = 0.2f;
		float m_waveLengthMax = 10.0f;

		size_t m_waveCount = 128;
		size_t m_gridWidth = 64;
		size_t m_gridHeight = 64;
		size_t m_indexCount;
		size_t m_vertexCount;
		std::vector < PrimitiveVertex > m_vertices;
		std::vector < uint32_t > m_indices;
		std::vector< GerstnerWave > m_gerstnerWaves;

	};
}
