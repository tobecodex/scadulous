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
  VkPhysicalDevice _device;
  std::vector<VkExtensionProperties> *_extensionProperties;
  std::vector<VkQueueFamilyProperties> *_queueFamilyProperties;
  std::map<VkSurfaceKHR, SwapChainProperties> _swapChainProperties;

public:
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

  VkPhysicalDevice device() const { return _device; }

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

  std::vector<uint32_t> getQueueFamilies(VkFlags flags)
  {
    std::vector<uint32_t> queueIndices;
    for (uint32_t i = 0; i < queueFamilyProperties().size(); i++) {
      if (queueFamilyProperties()[i].queueFlags & flags) {
        queueIndices.push_back(i);
      }
    }
    return queueIndices;
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

  std::vector<uint32_t> getPresentationQueues(VkSurfaceKHR surface)
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
  
  const std::vector<const char *> _validationLayers;

  std::vector<const char *> _requiredExtensions;
  std::vector<const char *> _deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

  VkDevice _logicalDevice;

  VulkanPhysicalDevice *_physicalDevice = nullptr;

  VkQueue _graphicsQueue;
  VkQueue _presentationQueue;

  VkSwapchainKHR _swapChain;
  VkExtent2D _swapChainExtent; 
  VkFormat _swapChainImageFormat;
  std::vector<VkImage> _swapChainImages;
  std::vector<VkImageView> _swapChainImageViews;

  std::optional<uint32_t> _graphicsFamily;
  std::optional<uint32_t> _presentationFamily;

  VkSurfaceKHR _surface;

  void createInstance();
  VulkanPhysicalDevice *selectPhysicalDevice();
  
  bool checkValidationLayers();
  void detectQueueFamilies();
  void createLogicalDevice();

  bool isDeviceSuitable(VulkanPhysicalDevice &device);

  void createSwapChain();
  void createImageViews();
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
  
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

