#pragma once
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

	class CommandBuffer;
	class VulkanPipeline
	{
	public:
		VulkanPipeline(const PipelineCreateInfo& desc);
		VulkanPipeline();
		~VulkanPipeline();

		virtual void Bind(CommandBuffer* commandList);
		virtual void Unbind(CommandBuffer* commandList);

		inline VkPipelineLayout GetPipelineLayout() { return m_pipelineLayout; }
		inline VkPipeline GetPipeline() {return m_pipeline; }
	private:

		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_pipeline;
		PipelineCreateInfo m_pipelineCreateInfo;
	};
}