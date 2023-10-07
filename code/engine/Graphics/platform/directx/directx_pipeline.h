#pragma once
#include "directx_pipeline_layout.h"
#include <vector>
namespace KNR
{
	enum class CullingMode;
	struct PipelineCreateInfo
	{
		D3D12_SHADER_BYTECODE vertShader; //vert shader
		D3D12_SHADER_BYTECODE pixelShader; 	//pixel shader
		D3D12_SHADER_BYTECODE computeShader; 	//compute shader
		D3D12_SHADER_BYTECODE hullShader; //hull shader
		D3D12_SHADER_BYTECODE domainShader; //domain shader
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType; 	//Topology type
		D3D12_INPUT_LAYOUT_DESC vertexInputLayout; 	//Vertex input Layout
		DirectXPipelineLayout pipelineLayout;
		CullingMode cullMode;
		bool wireframeModeEnabled;
		bool depthTestEnabled;
		std::vector<DXGI_FORMAT> rtvFormats;
	};

	class DirectXCommandBuffer;
	class DirectXPipeline
	{
	public:
		DirectXPipeline(const PipelineCreateInfo& desc);
		DirectXPipeline();
		~DirectXPipeline();

		virtual void Bind(DirectXCommandBuffer* commandList);
		virtual void Unbind(DirectXCommandBuffer* commandList);

		inline DirectXPipelineLayout* GetPipelineLayout() { return m_pipelineLayout; }
		inline ID3D12PipelineState* GetPipeline() {return m_pipelineState; }
	private:
		DirectXPipelineLayout* m_pipelineLayout; //root signature class
		ID3D12PipelineState* m_pipelineState;
		PipelineCreateInfo m_pipelineCreateInfo;
	};
}