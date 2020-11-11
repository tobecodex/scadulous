#ifndef __COMMAND_POOL_H
#define __COMMAND_POOL_H

#include <vector>
#include "vulkan/vulkan.h"

class CommandBuffer;

class CommandPool
{
private:
  VkCommandPool _commandPool;

public:
  CommandPool(uint32_t queueFamily);
  CommandPool(CommandPool &&other) = default;

  ~CommandPool();

  std::vector<CommandBuffer> createCommandBuffers(uint32_t numBuffers);
};

#endif