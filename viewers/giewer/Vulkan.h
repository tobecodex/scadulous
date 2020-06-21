#include <vulkan/vulkan.h>

#include <set>
#include <map>
#include <string>
#include <vector>
#include <optional>

class VulkanPhysicalDevice
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
  std::vector<VkExtensionProperties> *_extensionProperties;
  std::vector<VkQueueFamilyProperties> *_queueFamilyProperties;
  std::map<VkSurfaceKHR, SwapChainProperties> _swapChainProperties;

public:
  VkPhysicalDevice _device;
  
  VulkanPhysicalDevice(VkPhysicalDevice device) 
  : _device(device), _queueFamilyProperties(nullptr), _extensionProperties(nullptr)
  {
  }

  ~VulkanPhysicalDevice()
  {
    if (_queueFamilyProperties)
      delete _queueFamilyProperties;
    if (_extensionProperties)
      delete _extensionProperties;
  }

  const std::vector<VkExtensionProperties> &extensionProperties()
  {
    if (!_extensionProperties) {
      uint32_t extensionCount;
      vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);
      _extensionProperties = new std::vector<VkExtensionProperties>(extensionCount);
      vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, _extensionProperties->data());
    }
    return *_extensionProperties;
  }

  const std::vector<VkQueueFamilyProperties> &queueFamilyProperties()
  {
    if (!_queueFamilyProperties) {
      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);
      _queueFamilyProperties = new std::vector<VkQueueFamilyProperties>(queueFamilyCount);
      vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, _queueFamilyProperties->data());
    }
    return *_queueFamilyProperties;
  }

  const SwapChainProperties &swapChainProperties(VkSurfaceKHR surface)
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

  bool hasQueueFamilySupport(VkQueueFlags flags)
  {
    for (const auto &q : queueFamilyProperties()) {
      if (q.queueFlags & flags) {
        return true;
      }
    }
    return false;
  }

  bool hasPresentationSupport(VkSurfaceKHR surface)
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

  bool supportsExtensions(const std::vector<const char *> extensions)
  {
    std::set<std::string> required(extensions.begin(), extensions.end());
    for (const auto &available: extensionProperties()) {
      required.erase(available.extensionName);
    }
    return required.empty();
  }
};

class Vulkan
{
private:

  VkInstance _instance;
  std::vector<const char *> _requiredExtensions;
  const std::vector<const char *> _validationLayers;

  VkDevice _logicalDevice;
  VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
  std::vector<const char *> _deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

  std::optional<uint32_t> _graphicsFamily;
  std::optional<uint32_t> _presentFamily;
  VkQueue _graphicsQueue;
  VkQueue _presentQueue;
  
  VkSurfaceKHR _surface;

  void createInstance();
  VkPhysicalDevice selectPhysicalDevice();
  
  bool checkValidationLayers();
  void detectQueueFamilies();
  void createLogicalDevice();

  bool isDeviceSuitable(VulkanPhysicalDevice &device);

public:

  Vulkan(
    const std::vector<const char *> &extensions,
    void (*createSurfaceFn)(const VkInstance &, VkSurfaceKHR *, void *),
    void *createSurfaceFnUserData,
    const std::vector<const char *> &validationLayers = std::vector<const char *>()
  );
  ~Vulkan();

  VkSurfaceKHR &surface () { return _surface; } 
  const VkInstance &instance() const { return _instance; }
};

