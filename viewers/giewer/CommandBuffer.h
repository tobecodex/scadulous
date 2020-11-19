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
  void beginRecording();
  void endRecording();

  void beginRenderPass(uint32_t imageIndex, SwapChain &);
  void endRenderPass();

  operator VkCommandBuffer() { return _commandBuffer; }
};

#endif