#ifndef __VULKAN_H
#define __VULKAN_H

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

#include "Mesh.h"
#include "glm/glm.hpp"

class Device;
class UniformBuffer;
class ResourceBuffer;
class VertexBuffer;

struct Camera {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class Vulkan
{
private:

  static Vulkan *_currentContext;

  VkInstance _instance;
  VkSurfaceKHR _surface;
  Device *_device = nullptr;
  
  uint32_t _framesRendered = 0;
  std::vector<const char *> _extensions;
  std::vector<const char *> _validationLayers;

  void createSemaphores();
  VkSemaphore _imageAvailableSemaphore = nullptr, _renderFinishedSemaphore = nullptr;

  static VkInstance createInstance(
    const std::vector<const char *> &extensions, 
    const std::vector<const char *> &validationLayers
  );

  static bool checkValidationLayers(const std::vector<const char *> &validationLayers);

  UniformBuffer *_camera;

public:

  Vulkan(
    const std::vector<const char *> &extensions,
    const std::vector<const char *> &validationLayers = std::vector<const char *>()
  );
  ~Vulkan();

  static Vulkan &context() { return *_currentContext; }

  const std::vector<const char *> &extensions() const { return _extensions; }
  const std::vector<const char *> &validationLayers() const { return _validationLayers; }

  operator Device() const;
  operator VkDevice() const;
  operator VkPhysicalDevice() const;

  operator VkInstance() const; 
  operator VkSurfaceKHR() const;

  void surface(const VkSurfaceKHR &);

  void draw();

  VertexBuffer *createVertexBuffer(const std::vector<glm::vec3> &vertices);

  void addVertexShader(const std::string &path);
  void addFragmentShader(const std::string &path);
};

#endif