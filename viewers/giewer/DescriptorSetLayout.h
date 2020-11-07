#ifndef __DESCRIPTOR_SET_LAYOUT_H
#define __DESCRIPTOR_SET_LAYOUT_H

#include "Vulkan.h"

class DescriptorSetLayout
{
public:
  VkDescriptorSetLayout _descriptorSetLayout;

public:
  DescriptorSetLayout(const Device &, uint32_t loc, VkDescriptorType type, VkShaderStageFlags shaderStages);
  ~DescriptorSetLayout();

  operator VkDescriptorSetLayout() const { return _descriptorSetLayout; }
};

#endif