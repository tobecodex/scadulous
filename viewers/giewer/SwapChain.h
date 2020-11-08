#ifndef __SWAP_CHAIN_H
#define __SWAP_CHAIN_H

#include <vector>
#include "Vulkan.h"

class Device;

class SwapChain
{
private:
  VkSwapchainKHR _swapChain;

  VkExtent2D _extent; 
  VkFormat _imageFormat;
  VkRenderPass _renderPass;

  std::vector<VkImage> _images;
  std::vector<VkImageView> _imageViews;
  std::vector<VkFramebuffer> _frameBuffers;

  void createImageViews(const Device &);
  void createRenderPass(const Device &);
  void createFrameBuffers(const Device &);
  
  VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

public:
  SwapChain(const Device &, const VkSurfaceKHR &);
  ~SwapChain();

  uint32_t size() { return (uint32_t)_frameBuffers.size(); }

  operator VkSwapchainKHR() const { return _swapChain; }

  const VkExtent2D &extent() const { return _extent; }
  const VkFormat &imageFormat() const { return _imageFormat; }
  const VkRenderPass &renderPass() const { return _renderPass; }
  const std::vector<VkImage> &images() const { return _images; }
  const std::vector<VkFramebuffer> &frameBuffers() const { return _frameBuffers; }
};

#endif