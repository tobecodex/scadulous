#ifndef __COMMAND_POOL_H
#define __COMMAND_POOL_H

#include "Vulkan.h"

class SwapChain;
class DescriptorSet;
class GraphicsPipeline;
class CommandBuffer;

class CommandPool
{
private:
  VkCommandPool _commandPool;

public:
  CommandPool(Device &, uint32_t queueFamily);
  ~CommandPool();

  std::vector<CommandBuffer> *createCommandBuffers(Device &, uint32_t numBuffers);
};

#endif