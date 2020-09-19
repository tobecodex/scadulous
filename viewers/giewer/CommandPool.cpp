#include "CommandPool.h"

#include <stdexcept>

#include "SwapChain.h"
#include "DescriptorPool.h"
#include "GraphicsPipeline.h"

CommandPool::CommandPool(uint32_t queueFamily)
{
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamily;
  poolInfo.flags = 0;

  if (vkCreateCommandPool(Vulkan::context(), &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

CommandPool::~CommandPool()
{
  vkDestroyCommandPool(Vulkan::context(), _commandPool, nullptr);
}

CommandBuffer CommandPool::createCommandBuffer()
{
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = _commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  if (vkAllocateCommandBuffers(Vulkan::context(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  return CommandBuffer(commandBuffer);
}

void CommandBuffer::beginRecording(
  const GraphicsPipeline &graphicsPipeline,
  const DescriptorSet &descriptorSet,
  const SwapChain &swapChain,
  const VkFramebuffer &frameBuffer
)
{
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0; // Optional
  beginInfo.pInheritanceInfo = nullptr; // Optional

  if (vkBeginCommandBuffer(_commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = swapChain.renderPass();
  renderPassInfo.framebuffer = frameBuffer;
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChain.extent();

  VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

  std::vector<VkDescriptorSet> descriptorSets { descriptorSet };

  vkCmdBindDescriptorSets(
    _commandBuffer, 
    VK_PIPELINE_BIND_POINT_GRAPHICS, 
    graphicsPipeline.pipelineLayout(), 
    0, 1, descriptorSets.data(), 
    0, nullptr
  );
}

void CommandBuffer::endRecording()
{
  vkCmdEndRenderPass(_commandBuffer);

  if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}