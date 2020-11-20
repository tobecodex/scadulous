#include "CommandBuffer.h"
#include "SwapChain.h"
#include "GraphicsPipeline.h"

#include <stdexcept>

void CommandBuffer::beginRecording()
{
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0; // Optional
  beginInfo.pInheritanceInfo = nullptr; // Optional

  if (vkBeginCommandBuffer(_commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
}

void CommandBuffer::beginRenderPass(uint32_t imageIndex, SwapChain &swapChain)
{
  VkFramebuffer frameBuffer = swapChain.frameBuffers()[imageIndex];

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = swapChain.renderPass();
  renderPassInfo.framebuffer = frameBuffer;
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChain.extent();

  std::vector<VkClearValue> clearColors = {
    {0.0f, 0.0f, 0.0f, 1.0f}, { 1.0f, 0 }
  };

  renderPassInfo.clearValueCount = (uint32_t)clearColors.size();
  renderPassInfo.pClearValues = clearColors.data();

  vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::endRenderPass()
{
  vkCmdEndRenderPass(_commandBuffer);
}

void CommandBuffer::endRecording()
{

  if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}