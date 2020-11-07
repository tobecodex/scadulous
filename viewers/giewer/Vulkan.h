#ifndef __VULKAN_H
#define __VULKAN_H

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

#include "Mesh.h"
#include "CommandBuffer.h"

class Device;
class SwapChain;
class VertexBuffer;
class CommandBuffer;
class UniformBuffer;
class ResourceBuffer;
class GraphicsPipeline;
class CommandPool;
class CommandBuffer;
class DescriptorSet;
class DescriptorPool;
class DescriptorSetLayout;

class Vulkan
{
private:

  static Vulkan *_currentContext;

  VkInstance _instance;
  VkSurfaceKHR _surface;
  Device *_device = nullptr;
  
  void createSemaphores();
  VkSemaphore _imageAvailableSemaphore;
  VkSemaphore _renderFinishedSemaphore;

  uint32_t _framesRendered = 0;
  std::vector<const char *> _extensions;
  std::vector<const char *> _validationLayers;

  static VkInstance createInstance(
    const std::vector<const char *> &extensions, 
    const std::vector<const char *> &validationLayers
  );

  void createSwapChain();
  SwapChain *_swapChain = nullptr;

  GraphicsPipeline *_graphicsPipeline  = nullptr;
  void createGraphicsPipeline();

  DescriptorPool *_descriptorPool = nullptr;
  std::vector<DescriptorSet> *_descriptorSets = nullptr;
  std::vector<DescriptorSetLayout> *_descriptorSetLayouts = nullptr;

  static bool checkValidationLayers(const std::vector<const char *> &validationLayers);

  std::vector<UniformBuffer> *_camera;
  std::vector<VertexBuffer *> _geometry;
  VertexBuffer *createVertexBuffer(const std::vector<glm::vec3> &vertices);

  CommandPool *_commandPool = nullptr;
  std::vector<CommandBuffer> *_commandBuffers = nullptr;

public:

  Vulkan(
    const std::vector<const char *> &extensions,
    const std::vector<const char *> &validationLayers = std::vector<const char *>()
  );
  ~Vulkan();

  static Vulkan &context() { return *_currentContext; }

  const std::vector<const char *> &extensions() const { return _extensions; }
  const std::vector<const char *> &validationLayers() const { return _validationLayers; }

  operator VkDevice() const;
  operator VkInstance() const; 
  operator VkSurfaceKHR() const;
  operator VkPhysicalDevice() const;
  
  void surface(const VkSurfaceKHR &);

  void draw();

  void createCamera();
  void addMesh(const Mesh &);

  void addVertexShader(const std::string &path);
  void addFragmentShader(const std::string &path);
};

#endif