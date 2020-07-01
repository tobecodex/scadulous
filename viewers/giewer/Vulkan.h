#ifndef __VULKAN_H
#define __VULKAN_H

#include <vulkan/vulkan.h>

#include <set>
#include <map>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>

#include "Mesh.h"
#include "glm/glm.hpp"

#include "SwapChain.h"
#include "CommandQueues.h"
#include "PhysicalDevice.h"

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class Vulkan
{
private:
  
  VkDevice _device;
  VkInstance _instance;
  PhysicalDevice *_physicalDevice;

  VkPipeline _graphicsPipeline;
  VkDescriptorSetLayout _descriptorSetLayout;
  VkPipelineLayout _pipelineLayout;

  VkCommandPool _commandPool;
  std::vector<VkCommandBuffer> _commandBuffers;

  VkDescriptorPool _descriptorPool;
  std::vector<VkDescriptorSet> _descriptorSets;

  VkSurfaceKHR _surface;
  
  VkQueue _graphicsQueue;
  VkQueue _presentationQueue;

  void updateUniformBuffer(uint32_t);

  void createGraphicsPipeline();
  
  void createDescriptorSetLayout();
  static std::vector<char> loadShader(const std::string& filename);
  VkShaderModule createShaderModule(const std::vector<char>& shader); 

  void createBuffer(
    VkDeviceSize size, 
    VkBufferUsageFlags usage, 
    VkMemoryPropertyFlags properties, 
    VkBuffer& buffer, 
    VkDeviceMemory& bufferMemory
  ); 

  VkBuffer _vertexBuffer;
  VkDeviceMemory _vertexBufferMemory;
  void createVertexBuffer(const std::vector<glm::vec3> &vertices);

  std::vector<VkBuffer> _uniformBuffers;
  std::vector<VkDeviceMemory> _uniformBuffersMemory;
  void createUniformBuffers();

  void createCommandPool();
  void createCommandBuffers();

  void createDescriptorPool();
  void createDescriptorSets();

  void createSemaphores();
  VkSemaphore _imageAvailableSemaphore;
  VkSemaphore _renderFinishedSemaphore;

  SwapChain *_swapChain;
  CommandQueues *_queueFamilies;

  static VkInstance createInstance(
    const std::vector<const char *> &extensions, 
    const std::vector<const char *> &validationLayers
  );
  static bool checkValidationLayers(const std::vector<const char *> &);

  static PhysicalDevice *selectPhysicalDevice(
    VkInstance instance, VkSurfaceKHR surface, 
    const std::vector<const char *> &extensions
  );

  static bool isSuitableDevice(
    PhysicalDevice *device, 
    VkSurfaceKHR surface, 
    const std::vector<const char *> &extensions
  );

  static VkDevice createLogicalDevice(
    PhysicalDevice *physicalDevice,
    const std::vector<const char *> validationLayers, 
    const std::vector<uint32_t> &queueFamilies
  );

  uint32_t _framesRendered = 0;

public:

  Vulkan(
    const std::vector<const char *> &extensions,
    VkSurfaceKHR (*createSurfaceFn)(const VkInstance &, void *),
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