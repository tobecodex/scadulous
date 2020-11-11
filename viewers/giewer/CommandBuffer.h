#ifndef __COMMAND_BUFFER_H
#define __COMMAND_BUFFER_H

#include <vector>
#include <vulkan/vulkan.h>

class SwapChain;
class DescriptorSet;
class GraphicsPipeline;

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