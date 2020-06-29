#ifndef __VULKAN_H
#define __VULKAN_H

#include <vulkan/vulkan.h>

#include <set>
#include <map>
#include <string>
#include <vector>
#include <optional>

#include "Mesh.h"
#include "glm/glm.hpp"
#include "VulkanPhysicalDevice.h"

class Vulkan
{
private:

  VkInstance _instance;
  
  const std::vector<const char *> _validationLayers;

  std::vector<const char *> _requiredExtensions;
  std::vector<const char *> _deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

  VkDevice _device;

  VulkanPhysicalDevice *_physicalDevice = nullptr;

  VkQueue _graphicsQueue;
  VkQueue _presentationQueue;

  VkSwapchainKHR _swapChain;
  VkExtent2D _swapChainExtent; 
  VkFormat _swapChainImageFormat;
  std::vector<VkImage> _swapChainImages;
  std::vector<VkImageView> _swapChainImageViews;
  std::vector<VkFramebuffer> _swapChainFramebuffers;

  VkRenderPass _renderPass;
  VkPipeline _graphicsPipeline;
  VkPipelineLayout _pipelineLayout;

  VkCommandPool _commandPool;
  std::vector<VkCommandBuffer> _commandBuffers;

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

  void createRenderPass();
  void createGraphicsPipeline();
  
  static std::vector<char> loadShader(const std::string& filename);
  VkShaderModule createShaderModule(const std::vector<char>& shader); 

  void createFrameBuffers();

  VkBuffer _vertexBuffer;
  VkDeviceMemory _vertexBufferMemory;
  void createVertexBuffer(const std::vector<glm::vec3> &vertices);

  void createCommandPool();
  void createCommandBuffers();

  void createSemaphores();
  VkSemaphore _imageAvailableSemaphore;
  VkSemaphore _renderFinishedSemaphore;

  
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

  void addMesh(const Mesh &);
  void drawFrame();
};

#endif