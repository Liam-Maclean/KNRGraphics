#pragma once
#include "vulkan/vulkan.h"

#include <vector>
#include <array>
#include <string>

class VulkanGraphicsPipeline
{
public:
	VulkanGraphicsPipeline(char* vertShader, char* pixelShader);
	~VulkanGraphicsPipeline();

	VkPipeline GetGraphicsPipeline() { return m_GraphicsPipeline; };
	VkRenderPass GetRenderPass() { return m_RenderPass; };
	VkPipelineLayout GetPipelineLayout() { return m_GraphicsPipelineLayout; };

private:

	void Init();
	void CreateGraphicsPipeline();
	void CreateGraphicsPipelineLayout();
	void CreateDescriptorSetLayout();
	void CreateVertexDescriptor();
	void CreateRenderPass();

	VkPipelineShaderStageCreateInfo LoadShader(std::string fileName, VkShaderStageFlagBits stage);
	VkPipeline m_GraphicsPipeline;
	VkPipelineLayout m_GraphicsPipelineLayout;
	VkRenderPass m_RenderPass; 
	VkDescriptorSetLayout m_DescriptorSetLayout;

	VkPipelineVertexInputStateCreateInfo inputState;
	VkVertexInputBindingDescription bindingDescriptions;
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions;


	std::string m_VertShader, m_PixelShader;
};
