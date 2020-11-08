#include "Vulkan.h"

#include "Device.h"
#include "Camera.h"

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

#include "CommandPool.h"
#include "CommandBuffer.h"

#include "ResourceBuffer.h"
#include "DescriptorSet.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"

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
  if (_camera) {
    delete _camera;
  }

  vkDestroySemaphore(*_device, _imageAvailableSemaphore, nullptr);
  vkDestroySemaphore(*_device, _renderFinishedSemaphore, nullptr);

  if (_descriptorPool) {
    delete _descriptorPool;
  }
  if (_descriptorSetLayouts) {
    delete _descriptorSetLayouts;
  }
  if (_commandPool) {
    delete _commandPool;
  }
  if (_swapChain) {
    delete _swapChain;
  }
  if (_device) {
    delete _device;
  }
  vkDestroySurfaceKHR(_instance, _surface, nullptr);
  vkDestroyInstance(_instance, nullptr);
}

void Vulkan::createSwapChain()
{
  _swapChain = new SwapChain(*_device, _surface);
}

void Vulkan::createGraphicsPipeline()
{
  _descriptorPool = new DescriptorPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _swapChain->size());
  _descriptorSetLayouts = new std::vector<DescriptorSetLayout>();
  _descriptorSetLayouts->emplace_back(
    *_device, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT
  );

  _descriptorSets = _descriptorPool->createDescriptorSets(*_device, _swapChain->size(), *_descriptorSetLayouts);

  _cameraUniforms = new std::vector<UniformBuffer>;
  _cameraUniforms->reserve(_swapChain->size());

  for (int i = 0; i < _cameraUniforms->capacity(); i++) {
    _cameraUniforms->emplace_back(sizeof(ViewTransform), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    (*_descriptorSets)[i].bindResourceBuffer((*_cameraUniforms)[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  }

  _graphicsPipeline = new GraphicsPipeline(*_device, *_swapChain, *_descriptorSetLayouts);

  createSemaphores();
  _commandPool = new CommandPool(*_device, _device->graphicsFamily());

}

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

void Vulkan::setSurface(const VkSurfaceKHR &surface)
{
  _surface = surface;
  _device = new Device();

  createSwapChain();
  createGraphicsPipeline();

  _camera = new Camera(_swapChain->extent().width, _swapChain->extent().height);
}

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

void Vulkan::createSemaphores()
{
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(*_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
      vkCreateSemaphore(*_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS) {
    throw std::runtime_error("failed to create semaphores!");
  }
}

void Vulkan::draw()
{
  uint32_t imageIndex;
  vkAcquireNextImageKHR(
    *_device, *_swapChain, UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex
  );

  void* data;
  vkMapMemory(*_device, (*_cameraUniforms)[imageIndex], 0, sizeof(ViewTransform), 0, &data);
  memcpy(data, &_camera->transform(), sizeof(ViewTransform));
  vkUnmapMemory(*_device, (*_cameraUniforms)[imageIndex]);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore };
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;//_commandBuffers->size();
  std::vector<VkCommandBuffer> buffers{(*_commandBuffers)[imageIndex]};
  submitInfo.pCommandBuffers = &buffers[0];

  VkSemaphore signalSemaphores[] = { _renderFinishedSemaphore };
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(_device->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = { *_swapChain };
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr; // Optional

  vkQueuePresentKHR(_device->presentationQueue(), &presentInfo);
  vkQueueWaitIdle(_device->presentationQueue());
}

void Vulkan::addMesh(const Mesh &mesh)
{
  VertexBuffer *vb = createVertexBuffer(mesh.vertices());
  _geometry.push_back(vb);

  std::vector<VkBuffer> buffers{(VkBuffer)*vb};
  _commandBuffers = _commandPool->createCommandBuffers(*_device, _swapChain->size());
  for (int i = 0; i < _commandBuffers->size(); i++) {
    CommandBuffer &buffer = (*_commandBuffers)[i];
    std::vector<VkDescriptorSet> descriptorSets{(*_descriptorSets)[i]};
    buffer.beginRecording(i, *_swapChain, *_graphicsPipeline, *_descriptorSets);
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_graphicsPipeline);
    
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(buffer, 0, 1, buffers.data(), offsets);
    vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->pipelineLayout(), 0, 1, &descriptorSets[0], 0, nullptr);

    vkCmdDraw(buffer, (uint32_t)mesh.vertices().size(), 1, 0, 0);
    buffer.endRecording();
  }
}