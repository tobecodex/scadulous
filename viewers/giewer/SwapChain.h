#ifndef __SWAP_CHAIN_H
#define __SWAP_CHAIN_H

#include <vector>
#include <vulkan/vulkan.h>

#include "PhysicalDevice.h"

class SwapChain
{
private:
  VkDevice _device;
  VkSwapchainKHR _swapChain;

  VkExtent2D _extent; 
  VkFormat _imageFormat;
  VkRenderPass _renderPass;

  std::vector<VkImage> _images;
  std::vector<VkImageView> _imageViews;
  std::vector<VkFramebuffer> _frameBuffers;

  void createImageViews();
  void createRenderPass();
  void createFrameBuffers();
  
  VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

public:
  SwapChain(PhysicalDevice &, VkDevice &, VkSurfaceKHR);
  ~SwapChain();

  operator VkSwapchainKHR() const { return _swapChain; }

  const VkExtent2D &extent() const { return _extent; }
  const VkFormat &imageFormat() const { return _imageFormat; }
  const VkRenderPass &renderPass() const { return _renderPass; }
  const std::vector<VkFramebuffer> frameBuffers() const { return _frameBuffers; }
};

#endif