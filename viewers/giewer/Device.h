#ifndef __DEVICE_CONTEXT_H
#define __DEVICE_CONTEXT_H

#include "Vulkan.h"

#include "SwapChain.h"
#include "PhysicalDevice.h"
#include "GraphicsPipeline.h"

#include "CommandPool.h"

class Device
{
private:
  VkDevice _device = NULL;
  PhysicalDevice *_physicalDevice = nullptr;

  SwapChain *_swapChain = nullptr; 
  GraphicsPipeline *_graphicsPipeline = nullptr;

  uint32_t _graphicsFamily = -1;
  VkQueue _graphicsQueue = nullptr;

  uint32_t _presentationFamily = -1;
  VkQueue _presentationQueue = nullptr;

  CommandPool *_commandPool = nullptr;
  std::vector<CommandBuffer *> _commandBuffers;

  VkSemaphore _imageAvailableSemaphore;
  VkSemaphore _renderFinishedSemaphore;

  PhysicalDevice *selectPhysicalDevice(const std::vector<const char *> &extensions);
  bool isSuitableDevice(PhysicalDevice &, const std::vector<const char *> &extensions);

  void createSwapChain();
  void createSemaphores();
  void createGraphicsPipeline();
  void createLogicalDevice(const std::vector<uint32_t> &queueFamilies);

public:

  Device();
  ~Device();

  operator VkDevice() const { return _device; }
  operator VkPhysicalDevice() const { return *_physicalDevice; }

  const SwapChain &swapChain() { return *_swapChain; }
  const PhysicalDevice::SwapChainProperties &swapChainProperties() const;

  CommandBuffer createCommandBuffer();

  void draw();
};

#endif