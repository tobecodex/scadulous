#include "Vulkan.h"

#include <set>
#include <string>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <direct.h>

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

#include "Metrics.h"

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

  while (_semaphores.size()) {
    auto &s = _semaphores.front();
    vkDestroySemaphore(*_device, s.first, nullptr);
    vkDestroySemaphore(*_device, s.second, nullptr);
    _semaphores.pop();
  }

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

  _state.camera().setViewport((float)_swapChain->extent().width, (float)_swapChain->extent().height);
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
  _graphicsPipeline->addPushConstantRange();
  _graphicsPipeline->addShaderStage("shaders/shader.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
  _graphicsPipeline->addShaderStage("shaders/shader.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
  _graphicsPipeline->createLayout();
  _graphicsPipeline->createPipeline(_swapChain->renderPass());

  //_debugPipeline = new GraphicsPipeline(*_swapChain, _descriptorSetLayouts);
  //_graphicsPipeline->addShaderStage("shaders/shader.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
  //_graphicsPipeline->addShaderStage("shaders/shader.geom.spv", VK_SHADER_STAGE_VERTEX_BIT);
  //_graphicsPipeline->addShaderStage("shaders/shader.frag.spv", VK_SHADER_STAGE_VERTEX_BIT);
  //_graphicsPipeline->createLayout();
  //_graphicsPipeline->createPipeline();

  createFences();
  createSemaphores();

  _commandPools.reserve(_swapChain->size());
  _commandBuffers.reserve(_swapChain->size());

  for (uint32_t i = 0; i < _swapChain->size(); i++) {
    _commandPools.emplace_back(_device->graphicsFamily());
    _commandBuffers.emplace_back(_commandPools[i].createCommandBuffers(1));
  }

  _threadPool.push_back(std::thread(Vulkan::renderThread, this));
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
      fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

      vkCreateFence(*_device, &fenceInfo, nullptr, &_fences[i]);
  }
}

void Vulkan::createSemaphores()
{
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  for (int i = 0; i < (int)_swapChain->size(); i++) {
    VkSemaphore s1, s2;
    if (vkCreateSemaphore(*_device, &semaphoreInfo, nullptr, &s1) != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores!");
    }
    if (vkCreateSemaphore(*_device, &semaphoreInfo, nullptr, &s2) != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores!");
    }
    _semaphores.push({s1,s2});
  }
}

void Vulkan::draw()
{
  uint32_t imageIndex;

  auto &s = _semaphores.front();
  VkSemaphore &imageAvailable = s.first, &bufferComplete = s.second;

  vkAcquireNextImageKHR(
    *_device, *_swapChain, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex
  );

  _semaphores.push(_semaphores.front());
  _semaphores.pop();

  vkWaitForFences(*_device, 1, &_fences[imageIndex], true, UINT64_MAX);

  recordCommandBuffer(imageIndex);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &imageAvailable;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = (uint32_t)_commandBuffers[imageIndex].size();
  submitInfo.pCommandBuffers = (VkCommandBuffer *)_commandBuffers[imageIndex].data();

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &bufferComplete;

  vkResetFences(*_device, 1, &_fences[imageIndex]);
  if (vkQueueSubmit(_device->graphicsQueue(), 1, &submitInfo, _fences[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &bufferComplete;

  VkSwapchainKHR swapChains[] = { *_swapChain };
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr; // Optional

  vkQueuePresentKHR(_device->presentationQueue(), &presentInfo);

  metrics()["frames"]++;
  if (metrics()["frames"] % 10000 == 0) {
    dumpMetrics();
    metrics()["frames"] = 0;
  }
}

void Vulkan::renderThread(Vulkan *self)
{
  while (!self->_quitting) {
    self->draw();
  }
}

void Vulkan::recordCommandBuffer(uint32_t imageIndex)
{
  CommandBuffer buffer = _commandBuffers[imageIndex].front();
  buffer.beginRecording(imageIndex, *_swapChain, *_graphicsPipeline, _descriptorSets);

  vkCmdBindDescriptorSets(
    buffer, 
    VK_PIPELINE_BIND_POINT_GRAPHICS, 
    _graphicsPipeline->pipelineLayout(), 
    0, 1, (VkDescriptorSet *)&_descriptorSets[imageIndex], 
    0, nullptr
  );

  _state.lock();
 
  void* data;
  vkMapMemory(*_device, _cameraUniforms[imageIndex], 0, sizeof(ViewTransform), 0, &data);
  memcpy(data, &_state.camera().transform(), sizeof(ViewTransform));
  vkUnmapMemory(*_device, _cameraUniforms[imageIndex]);

  for (auto mesh : _state.meshes()) {

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
    vkCmdDraw(buffer, mesh->count(), 1, 0, 0);
  }

  buffer.endRecording(); 
  _state.unlock();
}

State &Vulkan::state() 
{
  return _state;
}

void Vulkan::addMesh(Mesh *m)
{
  m->createVertexBuffer();
  _state.meshes().push_back(m);
}