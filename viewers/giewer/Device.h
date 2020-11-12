#ifndef __DEVICE_H
#define __DEVICE_H

#include <vector>
#include <vulkan/vulkan.h>

#include "PhysicalDevice.h"

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
  Device(Device &&) = default;
  Device &operator =(Device &&) = default;
  ~Device();

  operator VkDevice() const;
  operator VkPhysicalDevice() const;

  uint32_t graphicsFamily() const; 
  uint32_t presentationFamily() const;

  VkQueue graphicsQueue() const;
  VkQueue presentationQueue() const;

  const PhysicalDevice::SwapChainProperties &swapChainProperties() const;
};

#endif