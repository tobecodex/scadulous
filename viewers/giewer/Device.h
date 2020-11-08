#ifndef __DEVICE_CONTEXT_H
#define __DEVICE_CONTEXT_H

#include "Vulkan.h"

#include "SwapChain.h"
#include "PhysicalDevice.h"
#include "GraphicsPipeline.h"

class Device
{
private:
  VkDevice _device = NULL;
  PhysicalDevice *_physicalDevice = nullptr;

  uint32_t _graphicsFamily = -1;
  VkQueue _graphicsQueue = nullptr;

  uint32_t _presentationFamily = -1;
  VkQueue _presentationQueue = nullptr;

  PhysicalDevice *selectPhysicalDevice(const std::vector<const char *> &extensions);
  bool isSuitableDevice(PhysicalDevice &, const std::vector<const char *> &extensions);

  void createLogicalDevice(const std::vector<uint32_t> &queueFamilies);

public:

  Device();
  ~Device();

  uint32_t graphicsFamily() const { return _graphicsFamily; }
  uint32_t presentationFamily() const { return _presentationFamily; }

  VkQueue graphicsQueue() const { return _graphicsQueue; }
  VkQueue presentationQueue() const { return _graphicsQueue; }

  operator VkDevice() const { return _device; }
  operator VkPhysicalDevice() const { return *_physicalDevice; }

  const PhysicalDevice::SwapChainProperties &swapChainProperties() const;
};

#endif