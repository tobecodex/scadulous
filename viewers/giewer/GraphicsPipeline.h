#ifndef __GRAPHICS_PIPELINE_H
#define __GRAPHICS_PIPELINE_H

#include <vector>
#include <vulkan/vulkan.h>

class Device;
class SwapChain;

class GraphicsPipeline
{
private:
  VkPipeline _graphicsPipeline = nullptr;
  VkPipelineLayout _pipelineLayout = nullptr;

public:
  GraphicsPipeline(const SwapChain &, std::vector<VkDescriptorSetLayout> &descriptorSetLayouts);
  ~GraphicsPipeline();

  operator VkPipeline() const { return _graphicsPipeline; }
  const VkPipelineLayout &pipelineLayout() const { return _pipelineLayout; }
};

#endif
