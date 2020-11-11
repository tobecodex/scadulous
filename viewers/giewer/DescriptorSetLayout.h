#ifndef __DESCRIPTOR_SET_LAYOUT_H
#define __DESCRIPTOR_SET_LAYOUT_H

#include <vulkan/vulkan.h>

class DescriptorSetLayout
{
public:
  VkDescriptorSetLayout _descriptorSetLayout;

public:
  DescriptorSetLayout(uint32_t loc, VkDescriptorType type, VkShaderStageFlags shaderStages);
  ~DescriptorSetLayout();

  operator VkDescriptorSetLayout() const { return _descriptorSetLayout; }
};

#endif