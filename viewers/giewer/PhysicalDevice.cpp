#include "PhysicalDevice.h"

#include <set>
#include <string>
#include <stdexcept>

const std::vector<VkExtensionProperties> &PhysicalDevice::extensionProperties()
{
  if (_extensionProperties.size() == 0) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);
    _extensionProperties.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, _extensionProperties.data());
  }
  return _extensionProperties;
}

const std::vector<VkQueueFamilyProperties> &PhysicalDevice::queueFamilyProperties()
{
  if (_queueFamilyProperties.size() == 0) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);
    _queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, _queueFamilyProperties.data());
  }
  return _queueFamilyProperties;
}

const PhysicalDevice::SwapChainProperties &PhysicalDevice::swapChainProperties(VkSurfaceKHR surface)
{
  if (_swapChainProperties.find(surface) == _swapChainProperties.end()) {

    SwapChainProperties &swapChain = _swapChainProperties[surface]; 
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, surface, &swapChain._capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(_device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
      swapChain._formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(_device, surface, &formatCount, swapChain._formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(_device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
      swapChain._presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(_device, surface, &presentModeCount, swapChain._presentModes.data());
    }
  }

  return _swapChainProperties[surface];
}

bool PhysicalDevice::hasQueueFamilySupport(VkQueueFlags flags)
{
  for (const auto &q : queueFamilyProperties()) {
    if (q.queueFlags & flags) {
      return true;
    }
  }
  return false;
}

std::vector<uint32_t> PhysicalDevice::getQueueFamilies(VkFlags flags)
{
  std::vector<uint32_t> queueIndices;
  for (uint32_t i = 0; i < queueFamilyProperties().size(); i++) {
    if (queueFamilyProperties()[i].queueFlags & flags) {
      queueIndices.push_back(i);
    }
  }
  return queueIndices;
}

bool PhysicalDevice::hasPresentationSupport(VkSurfaceKHR surface)
{
  for (int i = 0; i < queueFamilyProperties().size(); i++) {
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, surface, &presentSupport);
    if (presentSupport) {
      return true;
    }
  }
  return false;
}

std::vector<uint32_t> PhysicalDevice::getPresentationQueues(VkSurfaceKHR surface)
{
  std::vector<uint32_t> queueIndices;
  for (int i = 0; i < queueFamilyProperties().size(); i++) {
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, surface, &presentSupport);
    if (presentSupport) {
      queueIndices.push_back(i);
    }
  }
  return queueIndices;
}

bool PhysicalDevice::supportsExtensions(const std::vector<const char *> extensions)
{
  std::set<std::string> required(extensions.begin(), extensions.end());
  for (const auto &available: extensionProperties()) {
    required.erase(available.extensionName);
  }
  return required.empty();
}

uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(_device, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
  throw std::runtime_error("failed to find suitable memory type!");
}