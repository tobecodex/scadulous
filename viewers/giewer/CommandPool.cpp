#include "CommandPool.h"

#include <vector>
#include <stdexcept>

#include "Vulkan.h"
#include "SwapChain.h"
#include "CommandBuffer.h"
#include "DescriptorPool.h"
#include "GraphicsPipeline.h"

CommandPool::CommandPool(uint32_t queueFamily)
{
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamily;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  if (vkCreateCommandPool(Vulkan::ctx().device(), &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

CommandPool::~CommandPool()
{
  vkDestroyCommandPool(Vulkan::ctx().device(), _commandPool, nullptr);
}

std::vector<CommandBuffer> CommandPool::createCommandBuffers(uint32_t numBuffers)
{
  VkCommandBufferAllocateInfo allocInfo{};

  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = _commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = numBuffers;

  std::vector<CommandBuffer> commandBuffers;
  commandBuffers.resize(numBuffers);

  if (vkAllocateCommandBuffers(
    Vulkan::ctx().device(), &allocInfo, (VkCommandBuffer *)commandBuffers.data()) != VK_SUCCESS
  ) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  return commandBuffers;
}
