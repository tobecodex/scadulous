#ifndef __GRAPHICS_PIPELINE_H
#define __GRAPHICS_PIPELINE_H

#include <vector>
#include <vulkan/vulkan.h>

class Device;
class SwapChain;
class ShaderModule;

class GraphicsPipeline
{
private:
  VkPipelineLayoutCreateInfo _layoutInfo{};
  VkGraphicsPipelineCreateInfo _pipelineInfo{};
  VkPipelineViewportStateCreateInfo _viewportStateInfo{};
  VkPipelineColorBlendStateCreateInfo _colorBlendStateInfo{};
  VkPipelineVertexInputStateCreateInfo _vertexInputStateInfo{};
  VkPipelineMultisampleStateCreateInfo _multisampleStateInfo{};
  VkPipelineInputAssemblyStateCreateInfo _inputAssemblyStateInfo{};
  VkPipelineRasterizationStateCreateInfo _rasterizationStateInfo{};

  std::vector<VkRect2D> _scissors;
  std::vector<VkViewport> _viewports;
  std::vector<VkPushConstantRange> _pushConstantRanges;
  std::vector<VkDescriptorSetLayout> _descriptorSetLayouts;
  std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
  std::vector<VkPipelineColorBlendAttachmentState> _colorBlendAttachments;

  VkPipeline _graphicsPipeline = nullptr;
  VkPipelineLayout _pipelineLayout = nullptr;
  std::vector <ShaderModule *> _shaderModules;

  VkPipelineShaderStageCreateInfo createShaderStage(const char *, VkShaderStageFlagBits);

public:
  GraphicsPipeline(const SwapChain &, std::vector<VkDescriptorSetLayout> &descriptorSetLayouts);
  ~GraphicsPipeline();

  void createLayout();
  void createPipeline(VkRenderPass);
  void addPushConstantRange();
  void addShaderStage(const char *path, VkShaderStageFlagBits);

  operator VkPipeline() const { return _graphicsPipeline; }
  const VkPipelineLayout &pipelineLayout() const { return _pipelineLayout; }
};

#endif
