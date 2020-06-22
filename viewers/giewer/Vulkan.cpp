#include "Vulkan.h"

#include <set>
#include <string>
#include <stdexcept>
#include <algorithm>

Vulkan::Vulkan(
  const std::vector<const char *> &extensions, 
  void (*createSurfaceFn)(const VkInstance &, VkSurfaceKHR *, void *),
  void *createSurfaceFnUserData,
  const std::vector<const char *> &validationLayers
) : _requiredExtensions(extensions), _validationLayers(validationLayers)
{
  createInstance();
  createSurfaceFn(_instance, &_surface, createSurfaceFnUserData);
  _physicalDevice = selectPhysicalDevice();

  std::vector<uint32_t> intersection;
  std::vector<uint32_t> graphicsFamilies = _physicalDevice->getQueueFamilies(VK_QUEUE_GRAPHICS_BIT);
  std::vector<uint32_t> presentationFamilies = _physicalDevice->getPresentationQueues(_surface);

  std::sort(graphicsFamilies.begin(), graphicsFamilies.end()); 
  std::sort(presentationFamilies.begin(), presentationFamilies.end());

  std::set_intersection(
    graphicsFamilies.begin(), graphicsFamilies.end(),
    presentationFamilies.begin(), presentationFamilies.end(),
    std::back_inserter(intersection)
  );

  if (!intersection.empty()) {
    _graphicsFamily = _presentationFamily = intersection[0];
  }
  else {
    _graphicsFamily = graphicsFamilies[0];
    _presentationFamily = presentationFamilies[0];
  }

  createLogicalDevice();
  createSwapChain();
  createImageViews();
}

Vulkan::~Vulkan()
{
  for (auto imageView : _swapChainImageViews) {
    vkDestroyImageView(_logicalDevice, imageView, nullptr);
  }
  vkDestroySwapchainKHR(_logicalDevice, _swapChain, nullptr);
  vkDestroyDevice(_logicalDevice, nullptr);
  vkDestroySurfaceKHR(_instance, _surface, nullptr);
  vkDestroyInstance(_instance, nullptr);
}

void Vulkan::createInstance()
{
  VkApplicationInfo appInfo{};

  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hello Triangle";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  createInfo.enabledExtensionCount = (uint32_t) _requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = _requiredExtensions.data();

  createInfo.enabledLayerCount = (uint32_t)_validationLayers.size();
  if (createInfo.enabledLayerCount > 0 && checkValidationLayers()) {
    createInfo.ppEnabledLayerNames = _validationLayers.data();
  }

  // Create vk instance
  if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

bool Vulkan::checkValidationLayers()
{
  uint32_t availableLayerCount;
  vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(availableLayerCount);
  vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

  for (const char *validationLayer: _validationLayers) {
    bool found = false;
    for (const auto &layer : availableLayers) {
      if (strcmp(validationLayer, layer.layerName) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
       throw std::runtime_error("validation layers requested, but not available!");
    }
  }
  return true;
}

bool Vulkan::isDeviceSuitable(VulkanPhysicalDevice &device)
{
  bool swapChainOK = false;
  bool presentationOK = false;
  bool queueFamiliesOK = false;
  bool extensionsOK = device.supportsExtensions(_deviceExtensions);
   
  if (extensionsOK) {
    VulkanPhysicalDevice::SwapChainProperties swapChain = device.swapChainProperties(_surface);
    swapChainOK = (!swapChain._formats.empty() && !swapChain._presentModes.empty());
  }

  presentationOK = device.hasPresentationSupport(_surface);
  queueFamiliesOK = device.hasQueueFamilySupport(VK_QUEUE_GRAPHICS_BIT);
  
  return extensionsOK && swapChainOK && queueFamiliesOK && presentationOK;
}

VulkanPhysicalDevice *Vulkan::selectPhysicalDevice()
{
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
  vkEnumeratePhysicalDevices(_instance, &deviceCount, physicalDevices.data());
  std::vector<VulkanPhysicalDevice> devices(physicalDevices.begin(), physicalDevices.end());

  for (auto &device: devices) {
    if (isDeviceSuitable(device)) {
      return new VulkanPhysicalDevice(device.device());
    }
  }

  throw std::runtime_error("no suitable physical device found");
}


void Vulkan::createLogicalDevice()
{
  VkDeviceCreateInfo createInfo{};
  VkPhysicalDeviceFeatures deviceFeatures{};

  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = (uint32_t)(_deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

  if (_validationLayers.size() > 0) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
    createInfo.ppEnabledLayerNames = _validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = { _graphicsFamily.value(), _presentationFamily.value() };

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  if (vkCreateDevice(_physicalDevice->device(), &createInfo, nullptr, &_logicalDevice) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(_logicalDevice, _graphicsFamily.value(), 0, &_graphicsQueue);
  vkGetDeviceQueue(_logicalDevice, _presentationFamily.value(), 0, &_presentationQueue);
}

VkSurfaceFormatKHR Vulkan::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR Vulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
{
  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Vulkan::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
{
  if (capabilities.currentExtent.width != UINT32_MAX) {
      return capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = { 800, 600 };
    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
    return actualExtent;
  }
}

void Vulkan::createSwapChain() 
{
  const VulkanPhysicalDevice::SwapChainProperties &swapChain = _physicalDevice->swapChainProperties(_surface);

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChain._formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChain._presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChain._capabilities);

  uint32_t imageCount = swapChain._capabilities.minImageCount;
  if (swapChain._capabilities.maxImageCount > imageCount + 1) {
    imageCount += 1;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = _surface;

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (_graphicsFamily.value() != _presentationFamily.value()) {
    uint32_t queueFamilyIndices[] = { _graphicsFamily.value(), _presentationFamily.value() };

    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }
  
  createInfo.preTransform = swapChain._capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(_logicalDevice, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, nullptr);
  _swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, _swapChainImages.data());

  _swapChainImageFormat = surfaceFormat.format;
  _swapChainExtent = extent;
}

void Vulkan::createImageViews()
{
  _swapChainImageViews.resize(_swapChainImages.size());

  for (size_t i = 0; i < _swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = _swapChainImages[i];

    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = _swapChainImageFormat;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(_logicalDevice, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
}
