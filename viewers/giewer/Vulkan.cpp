#include "Vulkan.h"

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

#include "Mesh.h"

#include "Camera.h"
#include "Device.h"
#include "SwapChain.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "ResourceBuffer.h"
#include "DescriptorSet.h"
#include "DescriptorPool.h"
#include "ResourceBuffer.h"
#include "DescriptorSetLayout.h"
#include "GraphicsPipeline.h"

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
  _quitting = true;
  for (int i = 0; i < _threadPool.size(); i++) {
    _threadPool[i].join();
  }

  if (_camera) {
    delete _camera;
  }

  vkDestroySemaphore(*_device, _imageAvailableSemaphore, nullptr);
  vkDestroySemaphore(*_device, _renderFinishedSemaphore, nullptr);

  for (auto layout : _descriptorSetLayouts) {
    vkDestroyDescriptorSetLayout(*_device, layout, nullptr);
  }

  if (_swapChain) {
    delete _swapChain;
  }
  
  vkDestroySurfaceKHR(_instance, _surface, nullptr);
  vkDestroyInstance(_instance, nullptr);
}

Vulkan &Vulkan::ctx() { return *_currentContext; }

VkDevice Vulkan::device() const { return (VkDevice)(*_device); }
VkInstance Vulkan::instance() const { return _instance; }
VkSurfaceKHR Vulkan::surface() const { return _surface; }
VkPhysicalDevice Vulkan::physicalDevice() const { return (VkPhysicalDevice)(*_device); }

void Vulkan::setSurface(const VkSurfaceKHR &surface)
{
  _surface = surface;
  _device = new Device();

  createSwapChain();
  createGraphicsPipeline();

  _camera = new Camera((float)_swapChain->extent().width, (float)_swapChain->extent().height);
}

Camera &Vulkan::camera()
{
  return *_camera;
}

void Vulkan::createSwapChain()
{
  _swapChain = new SwapChain(*_device, _surface);
}

void Vulkan::createGraphicsPipeline()
{
  std::vector<VkDescriptorSetLayoutBinding> layoutBinding(1);
  for (int i = 0; i < layoutBinding.size(); i++) {
    layoutBinding[i].binding = i;
    layoutBinding[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding[i].descriptorCount = 1;
    layoutBinding[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding[i].pImmutableSamplers = nullptr;
  }

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = (uint32_t)layoutBinding.size();
  layoutInfo.pBindings = layoutBinding.data();

  _descriptorSetLayouts.resize(1);
  if (vkCreateDescriptorSetLayout(*_device, &layoutInfo, nullptr, _descriptorSetLayouts.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  std::vector<VkDescriptorSetLayout> layouts(_swapChain->size(), _descriptorSetLayouts[0]);
  _descriptorPool = new DescriptorPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (uint32_t)layouts.size());
  _descriptorSets = _descriptorPool->createDescriptorSets(layouts);

  _cameraUniforms.reserve(_swapChain->size());
  for (int i = 0; i < _descriptorSets.size(); i++) {

    VkDescriptorBufferInfo bufferInfo{};
  
    _cameraUniforms.emplace_back(
      sizeof(ViewTransform), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    bufferInfo.buffer = _cameraUniforms[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(glm::mat4);

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = _descriptorSets[i];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr; // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional

    vkUpdateDescriptorSets(*_device, 1, &descriptorWrite, 0, nullptr);
  }

  _graphicsPipeline = new GraphicsPipeline(*_swapChain, _descriptorSetLayouts);

  createFences();
  createSemaphores();
  
  _commandPool = new CommandPool(_device->graphicsFamily());
  _commandBuffers = _commandPool->createCommandBuffers(_swapChain->size());

  for (int i = 0; i < _swapChain->size(); i++) {
    _threadPool.push_back(
      std::thread(Vulkan::recordCommandBufferThread, std::pair<Vulkan *, uint32_t>(this, i))
    );
  }
}

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

void Vulkan::createFences()
{
  _fences.resize(_swapChain->size());
  for (int i = 0; i < _fences.size(); i++) {
      VkFenceCreateInfo fenceInfo{};

      fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceInfo.pNext = nullptr;
      fenceInfo.flags = 0;

      vkCreateFence(*_device, &fenceInfo, nullptr, &_fences[i]);
  }
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
  vkMapMemory(*_device, _cameraUniforms[imageIndex], 0, sizeof(ViewTransform), 0, &data);
  memcpy(data, &_camera->transform(), sizeof(ViewTransform));
  vkUnmapMemory(*_device, _cameraUniforms[imageIndex]);

  static unsigned int framesRendered = 0;
  for (auto m : _meshes) {
    m->transform(glm::rotate(m->transform(), 0.01f, glm::vec3(0,1,0)));
  }

  framesRendered++;

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &_imageAvailableSemaphore;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;//_commandBuffers->size();
  VkCommandBuffer buffer = _commandBuffers[imageIndex];
  submitInfo.pCommandBuffers = &buffer;

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &_renderFinishedSemaphore;

  if (vkQueueSubmit(_device->graphicsQueue(), 1, &submitInfo, _fences[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &_renderFinishedSemaphore;

  VkSwapchainKHR swapChains[] = { *_swapChain };
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr; // Optional

  vkQueuePresentKHR(_device->presentationQueue(), &presentInfo);
  vkQueueWaitIdle(_device->presentationQueue());
}

void Vulkan::recordCommandBufferThread(std::pair<Vulkan *, uint32_t> args)
{
  Vulkan *self = args.first;
  uint32_t idx = args.second;

  while (!self->_quitting) {
    self->recordCommandBuffer(idx);
  }
}

void Vulkan::recordCommandBuffer(uint32_t idx)
{
  CommandBuffer buffer = _commandBuffers[idx];
  buffer.beginRecording(idx, *_swapChain, *_graphicsPipeline, _descriptorSets);

  vkCmdBindDescriptorSets(
    buffer, 
    VK_PIPELINE_BIND_POINT_GRAPHICS, 
    _graphicsPipeline->pipelineLayout(), 
    0, 1, (VkDescriptorSet *)&_descriptorSets[idx], 
    0, nullptr
  );

  for (auto mesh : _meshes) {

    // Custom drawing

    vkCmdPushConstants(
      buffer,
      _graphicsPipeline->pipelineLayout(),
      VK_SHADER_STAGE_VERTEX_BIT,
      0,
      sizeof(mesh->transform()),
      &mesh->transform()
    );

    VkDeviceSize offsets[] = {0};
    VkBuffer vkBuffer = mesh->vkBuffer();
    vkCmdBindVertexBuffers(buffer, 0, 1, &vkBuffer, offsets);
    vkCmdDraw(buffer, (uint32_t)mesh->vertices().size(), 1, 0, 0);
  }
  buffer.endRecording(); 

  while (true) {
    VkResult result = vkWaitForFences(*_device, 1, &_fences[idx], true, 1000000);
    if (result == VK_SUCCESS) {
      break;
    }
  }
}

void Vulkan::addMesh(Mesh *m)
{
  m->createVertexBuffer();
  _meshes.push_back(m);
}