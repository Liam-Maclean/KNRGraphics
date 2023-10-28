#pragma once
#include <vector>
#include "directx11_util.h"
namespace KNR
{
	enum class CullingMode;
	struct PipelineCreateInfo
	{
		void* vertShader; //vert shader
		void* pixelShader; 	//pixel shader
		void* computeShader; 	//compute shader
		void* hullShader; //hull shader
		void* domainShader; //domain shader
		Topology topologyType; 	//Topology type
		void* vertexInputLayout; 	//Vertex input Layout
		CullingMode cullMode;
		bool wireframeModeEnabled;
		bool depthTestEnabled;
		std::vector<TextureFormat> rtvFormats;
	};

	class CommandBuffer;
	class DirectX11Pipeline
	{
	public:
		DirectX11Pipeline(const PipelineCreateInfo& desc);
		DirectX11Pipeline();
		~DirectX11Pipeline();

		virtual void Bind(CommandBuffer* commandList);
		virtual void Unbind(CommandBuffer* commandList);
	private:

		PipelineCreateInfo m_pipelineCreateInfo;
	};
}