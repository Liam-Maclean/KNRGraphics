#pragma once
#include "directx12_pipeline_layout.h"
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
		DirectX12PipelineLayout pipelineLayout;
		CullingMode cullMode;
		bool wireframeModeEnabled;
		bool depthTestEnabled;
		std::vector<DXGI_FORMAT> rtvFormats;
	};

	class DirectX12CommandBuffer;
	class DirectX12Pipeline
	{
	public:
		DirectX12Pipeline(const PipelineCreateInfo& desc);
		DirectX12Pipeline();
		~DirectX12Pipeline();

		virtual void Bind(DirectX12CommandBuffer* commandList);
		virtual void Unbind(DirectX12CommandBuffer* commandList);

		inline DirectX12PipelineLayout* GetPipelineLayout() { return m_pipelineLayout; }
		inline ID3D12PipelineState* GetPipeline() {return m_pipelineState; }
	private:
		DirectX12PipelineLayout* m_pipelineLayout; //root signature class
		ID3D12PipelineState* m_pipelineState;
		PipelineCreateInfo m_pipelineCreateInfo;
	};
}