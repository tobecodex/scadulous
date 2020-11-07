#ifndef __GRAPHICS_PIPELINE_H
#define __GRAPHICS_PIPELINE_H

#include "Vulkan.h"

class Device;
class SwapChain;
class DescriptorSetLayout;

class GraphicsPipeline
{
private:
  VkPipeline _graphicsPipeline = nullptr;
  VkPipelineLayout _pipelineLayout = nullptr;

public:
  GraphicsPipeline(const Device &, const SwapChain &, std::vector<DescriptorSetLayout> &descriptorSetLayouts);
  ~GraphicsPipeline();

  operator VkPipeline() const { return _graphicsPipeline; }
  const VkPipelineLayout &pipelineLayout() const { return _pipelineLayout; }
};

#endif