#ifndef __COMMAND_BUFFER_H
#define __COMMAND_BUFFER_H

#include "Vulkan.h"

class GraphicsPipeline;
class DescriptorSet;
class SwapChain;

class CommandBuffer
{
private:
  VkCommandBuffer _commandBuffer;

public:
  void beginRecording(int, SwapChain &, GraphicsPipeline &, std::vector<DescriptorSet> &);
  void endRecording();

  operator VkCommandBuffer() { return _commandBuffer; }
};

#endif