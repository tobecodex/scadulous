#include "Vulkan.h"

#include <set>
#include <string>
#include <stdexcept>

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

  createLogicalDevice();
}

Vulkan::~Vulkan()
{
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
  bool extensionsOK = device.supportsExtensions(_deviceExtensions);
  
  if (extensionsOK) {
    VulkanPhysicalDevice::SwapChainProperties swapChain = device.swapChainProperties(_surface);
  }

  return extensionsOK;
}

VkPhysicalDevice Vulkan::selectPhysicalDevice()
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
      return device._device;
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
  std::set<uint32_t> uniqueQueueFamilies = {_graphicsFamily.value(), _presentFamily.value()};

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

  if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_logicalDevice) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(_logicalDevice, _graphicsFamily.value(), 0, &_graphicsQueue);
  vkGetDeviceQueue(_logicalDevice, _presentFamily.value(), 0, &_presentQueue);
}