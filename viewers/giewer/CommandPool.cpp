#include "CommandPool.h"

#include <vector>
#include <stdexcept>

#include "SwapChain.h"
#include "DescriptorPool.h"
#include "GraphicsPipeline.h"
#include "CommandBuffer.h"

CommandPool::CommandPool(Device &device, uint32_t queueFamily)
{
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamily;
  poolInfo.flags = 0;

  if (vkCreateCommandPool(device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

CommandPool::~CommandPool()
{
  vkDestroyCommandPool(Vulkan::context(), _commandPool, nullptr);
}

std::vector<CommandBuffer> *CommandPool::createCommandBuffers(Device &device, uint32_t numBuffers)
{
  VkCommandBufferAllocateInfo allocInfo{};

  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = _commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = numBuffers;

  auto commandBuffers = new std::vector<CommandBuffer>();
  commandBuffers->resize(numBuffers);

  if (vkAllocateCommandBuffers(device, &allocInfo, (VkCommandBuffer *)commandBuffers->data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  return commandBuffers;
}
