#include "DescriptorPool.h"

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

  if (vkCreateDescriptorPool(Vulkan::context(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

DescriptorPool::~DescriptorPool()
{
  vkDestroyDescriptorPool(Vulkan::context(), _descriptorPool, nullptr);
}

std::vector<DescriptorSet> *DescriptorPool::createDescriptorSets(Device &device, uint32_t numSets, const std::vector<DescriptorSetLayout> &layouts)
{
  std::vector<VkDescriptorSetLayout> _layouts(numSets, layouts.front());

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = _descriptorPool;
  allocInfo.descriptorSetCount = numSets;
  allocInfo.pSetLayouts = _layouts.data();

  auto descriptorSets = new std::vector<DescriptorSet>(numSets) ;
  if (vkAllocateDescriptorSets(device, &allocInfo, (VkDescriptorSet *)descriptorSets->data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  return descriptorSets;
}

// ##

DescriptorSetLayout::DescriptorSetLayout(const Device &device, uint32_t loc, VkDescriptorType type, VkShaderStageFlags shaderStages)
{
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = loc;
  layoutBinding.descriptorType = type;
  layoutBinding.descriptorCount = 1;
  layoutBinding.stageFlags = shaderStages;
  layoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &layoutBinding;

  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

DescriptorSetLayout::~DescriptorSetLayout()
{
  vkDestroyDescriptorSetLayout(Vulkan::context(), _descriptorSetLayout, nullptr);
}
