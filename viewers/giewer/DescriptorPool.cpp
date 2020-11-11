#include "DescriptorPool.h"

#include "Vulkan.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"

#include <stdexcept>

DescriptorPool::DescriptorPool(VkDescriptorType type, uint32_t maxSets)
{
  VkDescriptorPoolSize poolSize{};
  poolSize.type = type;
  poolSize.descriptorCount = 1;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;

  poolInfo.maxSets = maxSets;

  if (vkCreateDescriptorPool(Vulkan::ctx().device(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

DescriptorPool::~DescriptorPool()
{
  vkDestroyDescriptorPool(Vulkan::ctx().device(), _descriptorPool, nullptr);
}

std::vector<DescriptorSet> DescriptorPool::createDescriptorSets(const std::vector<VkDescriptorSetLayout> &layouts)
{
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = _descriptorPool;
  allocInfo.descriptorSetCount = (uint32_t)layouts.size();
  allocInfo.pSetLayouts = layouts.data();

  std::vector<DescriptorSet> descriptorSets;
  descriptorSets.resize(layouts.size());

  if (vkAllocateDescriptorSets(Vulkan::ctx().device(), &allocInfo, (VkDescriptorSet *)descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  return descriptorSets;
}