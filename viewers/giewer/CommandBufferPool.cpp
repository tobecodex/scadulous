#include "CommandBufferPool.h"

CommandBufferPool::CommandBufferPool(uint32_t queueFamily) : _commandPool(queueFamily)
{
}

std::vector<CommandBuffer> &CommandBufferPool::inUse()
{
  return _inUse;
}

CommandBuffer &CommandBufferPool::acquire()
{
  if (_available.size() == 0) {
    auto newBuffers = _commandPool.createCommandBuffers(1);
    std::move(newBuffers.begin(), newBuffers.end(), std::back_inserter(_available));
  }
  _inUse.push_back(_available.back());
  _available.pop_back();
  return _inUse.back();
}

void CommandBufferPool::reset()
{
  std::move(_inUse.begin(), _inUse.end(), std::back_inserter(_available));
  _inUse.clear();
}
