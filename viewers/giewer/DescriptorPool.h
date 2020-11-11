#ifndef __DESCRIPTOR_POOL_H
#define __DESCRIPTOR_POOL_H

#include <vector>
#include <vulkan/vulkan.h>

class DescriptorSet;
class DescriptorSetLayout;

class DescriptorPool
{
private:
  VkDescriptorPool _descriptorPool;

public:
  DescriptorPool(DescriptorPool &&) = default;
  DescriptorPool &operator=(DescriptorPool &&) = default;
  DescriptorPool(VkDescriptorType type, uint32_t maxSets);
  ~DescriptorPool();

  std::vector<DescriptorSet> createDescriptorSets(const std::vector<VkDescriptorSetLayout> &);
};

#endif
