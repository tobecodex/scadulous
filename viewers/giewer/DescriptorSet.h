#ifndef __DESCRIPTOR_SET_H
#define __DESCRIPTOR_SET_H

#include "Vulkan.h"

class ResourceBuffer;

class DescriptorSet
{
private:
  VkDescriptorSet _descriptorSet = nullptr;

public:
  DescriptorSet();
  DescriptorSet(VkDescriptorSet &descriptorSet);

  void bindResourceBuffer(ResourceBuffer &buffer, VkDescriptorType descriptorType);

  operator VkDescriptorSet();
};

#endif