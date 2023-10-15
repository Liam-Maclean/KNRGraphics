#pragma once
#include <vector>
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
		VkPipelineLayout pipelineLayout;
		CullingMode cullMode;
		bool wireframeModeEnabled;
		bool depthTestEnabled;
		std::vector<TextureFormat> rtvFormats;
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