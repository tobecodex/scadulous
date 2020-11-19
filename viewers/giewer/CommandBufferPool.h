#ifndef __COMMAND_BUFFER_POOL_H
#define __COMMAND_BUFFER_POOL_H

#include "CommandPool.h"
#include "CommandBuffer.h"

class CommandBufferPool
{
private:
  CommandPool _commandPool;
  std::vector<CommandBuffer> _inUse, _available;

public:
  CommandBufferPool(uint32_t queueFamily);

  void reset();
  CommandBuffer &acquire();
  std::vector<CommandBuffer> &inUse();
};

#endif