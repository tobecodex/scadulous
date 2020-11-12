#ifndef __VULKAN_H
#define __VULKAN_H

#include <vulkan/vulkan.h>

#include <thread>
#include <vector>
#include <string>
#include <optional>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class Mesh;
class Camera;
class Device; 

class SwapChain;

class CommandPool;
class CommandBuffer;

class GraphicsPipeline;

class VertexBuffer;
class UniformBuffer;

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

  void createFences();  
  void createSemaphores();

  std::vector<VkFence> _fences;
  VkSemaphore _imageAvailableSemaphore;
  VkSemaphore _renderFinishedSemaphore;

  bool _quitting = false;
  std::vector<std::thread> _threadPool;
  void recordCommandBuffer(uint32_t idx);
  static void recordCommandBufferThread(std::pair<Vulkan *, uint32_t>);

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
  std::vector<DescriptorSet> _descriptorSets;
  std::vector<VkDescriptorSetLayout> _descriptorSetLayouts;

  static bool checkValidationLayers(const std::vector<const char *> &validationLayers);

  Camera *_camera = nullptr;

  std::vector<UniformBuffer> _cameraUniforms;

  std::vector<Mesh *> _meshes;
  VertexBuffer *createVertexBuffer(const std::vector<glm::vec3> &vertices);

  CommandPool *_commandPool = nullptr;
  std::vector<CommandBuffer> _commandBuffers;


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
  void addMesh(Mesh *);
  Camera &camera();

  void addVertexShader(const std::string &path);
  void addFragmentShader(const std::string &path);
};

#endif