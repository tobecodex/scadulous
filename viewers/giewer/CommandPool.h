#ifndef __COMMAND_POOL_H
#define __COMMAND_POOL_H

#include "Vulkan.h"

class SwapChain;
class DescriptorSet;
class GraphicsPipeline;

class CommandBuffer
{
private:
  const VkCommandBuffer &_commandBuffer;

public:
  CommandBuffer(const VkCommandBuffer &commandBuffer) : _commandBuffer(commandBuffer) {}
  
  void beginRecording(const GraphicsPipeline &, const DescriptorSet &, const SwapChain &, const VkFramebuffer &);
  void endRecording();
};

class CommandPool
{
private:
  VkCommandPool _commandPool;

public:
  CommandPool(uint32_t queueFamily);
  ~CommandPool();

  CommandBuffer createCommandBuffer();
};

#endif