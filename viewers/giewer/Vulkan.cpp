#include "Vulkan.h"

#include "Device.h"

#include <set>
#include <string>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <direct.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "ResourceBuffer.h"

Vulkan *Vulkan::_currentContext = nullptr;

Vulkan::Vulkan(
  const std::vector<const char *> &extensions, 
  const std::vector<const char *> &validationLayers
) : _extensions(extensions), _validationLayers(validationLayers)
{
  _instance = createInstance(_extensions, _validationLayers);
  _currentContext = this;
}

Vulkan::~Vulkan()
{
  //vkDestroySemaphore(_device, _renderFinishedSemaphore, nullptr);
  //vkDestroySemaphore(_device, _imageAvailableSemaphore, nullptr);

  //vkDestroyDevice(_device, nullptr);
  vkDestroySurfaceKHR(_instance, _surface, nullptr);
  vkDestroyInstance(_instance, nullptr);
}


Vulkan::operator Device() const { return *_device; }
Vulkan::operator VkDevice() const { return *_device; }
Vulkan::operator VkPhysicalDevice() const { return *_device; }
Vulkan::operator VkInstance() const { return _instance; }
Vulkan::operator VkSurfaceKHR() const { return _surface; }

bool Vulkan::checkValidationLayers(const std::vector<const char *> &validationLayers)
{
  uint32_t availableLayerCount;
  vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(availableLayerCount);
  vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

  for (const char *validationLayer: validationLayers) {
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

VkInstance Vulkan::createInstance(
  const std::vector<const char *> &extensions, 
  const std::vector<const char *> &validationLayers
)
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

  createInfo.enabledExtensionCount = (uint32_t) extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();

  createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
  if (createInfo.enabledLayerCount > 0 && checkValidationLayers(validationLayers)) {
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  // Create vk instance
  VkInstance instance;
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }

  return instance;
}

void Vulkan::surface(const VkSurfaceKHR &surface)
{
  _surface = surface;
  _device = new Device();
}

/*void Vulkan::createCamera() 
{
  VkDeviceSize bufferSize = sizeof(Camera);

  _camera = new ResourceBuffer(
    sizeof(Camera), 
    VK_BUFFER_UNIFORM_BUFFER_BIT, 
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
}*/

VertexBuffer *Vulkan::createVertexBuffer(const std::vector<glm::vec3> &vertices)
{
  size_t bufSize = sizeof(vertices[0]) * vertices.size();

  VertexBuffer *buffer = new VertexBuffer(
    bufSize,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );

  void* data;
  if (vkMapMemory(*_device, (VkDeviceMemory)*buffer, 0, bufSize, 0, &data) != VK_SUCCESS) {
    throw std::runtime_error("failed to map vertex buffer!");
  }
  memcpy(data, vertices.data(), bufSize);
  vkUnmapMemory(*_device, *buffer);

  return buffer;
}

void Vulkan::draw()
{
  _device->draw();
}

/*
void Vulkan::addMesh(const Mesh &mesh)
{
  createVertexBuffer(mesh.vertices());
  createCommandBuffers();
}*/

