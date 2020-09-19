#include "Device.h"

#include <set>
#include <iterator>
#include <algorithm>

Device::Device()
{
  _physicalDevice = selectPhysicalDevice({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
  
  auto graphicsFamilies = _physicalDevice->getQueueFamilies(VK_QUEUE_GRAPHICS_BIT);
  auto presentationFamilies = _physicalDevice->getPresentationQueues(Vulkan::context());

  if (graphicsFamilies.size() == 0) {
    throw std::runtime_error("Could not find a graphics queue");
  }
  
  if (presentationFamilies.size() == 0) {
    throw std::runtime_error("Could not find a graphics queue");
  }

  // See if there are any graphics + presentation families
  std::vector<uint32_t> queueFamilies;
  std::sort(graphicsFamilies.begin(), graphicsFamilies.end());
  std::sort(presentationFamilies.begin(), presentationFamilies.end());
  std::set_intersection(
    graphicsFamilies.begin(), graphicsFamilies.end(), 
    presentationFamilies.begin(), presentationFamilies.end(),
    std::back_inserter(queueFamilies)
  );

  if (queueFamilies.size() == 0) {
    // No combined queue, use the separate ones
    _graphicsFamily = graphicsFamilies[0];
    _presentationFamily = presentationFamilies[0];
  } else {
    // We only need a single combined queue
    _graphicsFamily = _presentationFamily = queueFamilies[0];
  }
  queueFamilies = { _graphicsFamily, _presentationFamily };

  createLogicalDevice(queueFamilies);
  createSwapChain();
  createGraphicsPipeline();
}

Device::~Device()
{
}

const PhysicalDevice::SwapChainProperties &Device::swapChainProperties() const
{
  return _physicalDevice->swapChainProperties(Vulkan::context());
}

bool Device::isSuitableDevice(PhysicalDevice &device, const std::vector<const char *> &extensions)
{
  bool swapChainOK = false;
  bool presentationOK = false;
  bool queueFamiliesOK = false;
  bool extensionsOK = device.supportsExtensions(extensions);
   
  if (extensionsOK) {
    const PhysicalDevice::SwapChainProperties &swapChain = device.swapChainProperties(Vulkan::context());
    swapChainOK = (!swapChain._formats.empty() && !swapChain._presentModes.empty());
  }

  presentationOK = device.hasPresentationSupport(Vulkan::context());
  queueFamiliesOK = device.hasQueueFamilySupport(VK_QUEUE_GRAPHICS_BIT);
  
  return extensionsOK && swapChainOK && queueFamiliesOK && presentationOK;
}

PhysicalDevice *Device::selectPhysicalDevice(const std::vector<const char *> &extensions)
{
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(Vulkan::context(), &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
  vkEnumeratePhysicalDevices(Vulkan::context(), &deviceCount, physicalDevices.data());
  std::vector<PhysicalDevice> devices(physicalDevices.begin(), physicalDevices.end());


  for (auto &device: devices) {
    if (isSuitableDevice(device, extensions)) {
      return new PhysicalDevice(device);
    }
  }

  throw std::runtime_error("no suitable physical device found");
}


void Device::createLogicalDevice(const std::vector<uint32_t> &queueFamilies)
{ 
  VkDeviceCreateInfo createInfo{};
  VkPhysicalDeviceFeatures deviceFeatures{};
  std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = (uint32_t)(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  createInfo.enabledLayerCount = static_cast<uint32_t>(Vulkan::context().validationLayers().size());
  if (Vulkan::context().validationLayers().size() > 0) {
    createInfo.ppEnabledLayerNames = Vulkan::context().validationLayers().data();
  }

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies(queueFamilies.begin(), queueFamilies.end());

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  if (vkCreateDevice(*_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
}

void Device::createSwapChain()
{
  _swapChain = new SwapChain(*this, Vulkan::context());
}

void Device::createGraphicsPipeline()
{
  _graphicsPipeline = new GraphicsPipeline(*this, *_swapChain);
}

void Device::createSemaphores()
{
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
      vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS) {
    throw std::runtime_error("failed to create semaphores!");
  }
}

CommandBuffer Device::createCommandBuffer()
{
  if (!_commandPool) {
    _commandPool = new CommandPool(_presentationFamily);
  }

  return _commandPool->createCommandBuffer();
}

/*
void Device::updateUniformBuffer(uint32_t currentImage)
{
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  UniformBufferObject ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt({1, 1, 1}, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(45.0f), _swapChain->extent().width / (float)_swapChain->extent().height, 0.1f, 10.0f);
  ubo.proj[1][1] *= -1;

  void* data;
  vkMapMemory(_device, _uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));

}*/

void Device::draw()
{
  uint32_t imageIndex;
  vkAcquireNextImageKHR(
    _device, *_swapChain, UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex
  );

  //updateUniformBuffer(imageIndex);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore };
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 0;//_device->commandBuffers().;
  submitInfo.pCommandBuffers = nullptr;//_device->commandBuffers(imageIndex);

  VkSemaphore signalSemaphores[] = { _renderFinishedSemaphore };
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
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

  vkQueuePresentKHR(_presentationQueue, &presentInfo);
  vkQueueWaitIdle(_presentationQueue);

 // _framesRendered++;
}
