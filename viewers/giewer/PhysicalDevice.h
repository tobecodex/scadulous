#ifndef __PHYSICAL_DEVICE_H
#define __PHYSICAL_DEVICE_H

#include <map>
#include <vector>
#include <vulkan/vulkan.h>

class PhysicalDevice
{
public:
  class SwapChainProperties
  {
  public:
    VkSurfaceCapabilitiesKHR _capabilities;
    std::vector<VkSurfaceFormatKHR> _formats;
    std::vector<VkPresentModeKHR> _presentModes;  
  };

private:
  VkPhysicalDevice _device;
  std::vector<VkExtensionProperties> _extensionProperties;
  std::vector<VkQueueFamilyProperties> _queueFamilyProperties;
  std::map<VkSurfaceKHR, SwapChainProperties> _swapChainProperties;

public:
  PhysicalDevice(VkPhysicalDevice device) : _device(device) {}

  operator VkPhysicalDevice() const { return _device; }

  const std::vector<VkExtensionProperties> &extensionProperties();
  const std::vector<VkQueueFamilyProperties> &queueFamilyProperties();
  const SwapChainProperties &swapChainProperties(VkSurfaceKHR);
  bool hasQueueFamilySupport(VkQueueFlags flags);
  std::vector<uint32_t> getQueueFamilies(VkFlags flags);
  bool hasPresentationSupport(VkSurfaceKHR surface);
  std::vector<uint32_t> getPresentationQueues(VkSurfaceKHR surface);
  bool supportsExtensions(const std::vector<const char *> extensions);
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

#endif