#ifndef __DESCRIPTOR_H
#define __DESCRIPTOR_H

#include "Vulkan.h"
#include "Device.h"
#include "ResourceBuffer.h"

#include <stdexcept>

class Device;
class DescriptorSet;
class DescriptorSetLayout;

class DescriptorPool
{
private:
  VkDescriptorPool _descriptorPool;

public:
  DescriptorPool(VkDescriptorType type, uint32_t maxSets);
  ~DescriptorPool();

  std::vector<DescriptorSet> *createDescriptorSets(Device &, uint32_t numSets, const std::vector<DescriptorSetLayout> &);
};

#endif
