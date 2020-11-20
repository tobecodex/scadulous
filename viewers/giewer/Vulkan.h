#ifndef __VULKAN_H
#define __VULKAN_H

#include <vulkan/vulkan.h>

#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <string>

#include "State.h"
#include "CommandBufferPool.h"

class Mesh;
class Camera;
class Device; 

class SwapChain;

class GraphicsPipeline;

class VertexBuffer;
class UniformBuffer;

class DescriptorSet;
class DescriptorPool;
class DescriptorSetLayout;

class Vulkan
{
private:

  State _state;

  static Vulkan *_currentContext;

  VkInstance _instance;
  VkSurfaceKHR _surface;

  Device *_device = nullptr;

  void createFences();  
  void createSemaphores();

  std::vector<VkFence> _fences;
  std::queue<std::pair<VkSemaphore, VkSemaphore>> _semaphores;

  bool _quitting = false;
  std::vector<std::thread> _threadPool;
  void recordCommandBuffer(uint32_t idx);
  static void renderThread(Vulkan *self);

  uint64_t _framesRendered = 0;
  std::vector<const char *> _extensions;
  std::vector<const char *> _validationLayers;

  static VkInstance createInstance(
    const std::vector<const char *> &extensions, 
    const std::vector<const char *> &validationLayers
  );

  void createSwapChain();
  SwapChain *_swapChain = nullptr;

  GraphicsPipeline *_graphicsPipeline  = nullptr;
  GraphicsPipeline *_debugPipeline = nullptr;

  void createGraphicsPipeline();

  DescriptorPool *_descriptorPool = nullptr;
  std::vector<DescriptorSet> _descriptorSets;
  std::vector<VkDescriptorSetLayout> _descriptorSetLayouts;

  static bool checkValidationLayers(const std::vector<const char *> &validationLayers);

  std::vector<UniformBuffer> _cameraUniforms;

  VertexBuffer *createVertexBuffer(const std::vector<glm::vec3> &vertices);

  bool _debugDraw = false;
  std::vector<CommandBufferPool> _commandBufferPools;

public:

  Vulkan(
    const std::vector<const char *> &extensions,
    const std::vector<const char *> &validationLayers = std::vector<const char *>()
  );
  ~Vulkan();

  static Vulkan &ctx();

  VkDevice device() const; 
  VkInstance instance() const; 
  VkSurfaceKHR surface() const;
  VkPhysicalDevice physicalDevice() const;

  const std::vector<const char *> &extensions() const { return _extensions; }
  const std::vector<const char *> &validationLayers() const { return _validationLayers; }

  void setSurface(const VkSurfaceKHR &);

  void draw();
  void toggleDebugDraw();

  State &state();
  void addMesh(Mesh *);

  void addVertexShader(const std::string &path);
  void addFragmentShader(const std::string &path);
};

#endif