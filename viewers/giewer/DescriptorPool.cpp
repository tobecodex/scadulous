#include "DescriptorPool.h"

#include <stdexcept>

DescriptorPool::DescriptorPool(VkDescriptorType type)
{
  VkDescriptorPoolSize poolSize{};
  poolSize.type = type;
  poolSize.descriptorCount = 1;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;

  poolInfo.maxSets = static_cast<uint32_t>(1);

  if (vkCreateDescriptorPool(Vulkan::context(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

DescriptorSet DescriptorPool::createDescriptorSet(const DescriptorSetLayout &layout)
{
  std::vector<VkDescriptorSetLayout> layouts { layout };

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = _descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layouts.data();

  VkDescriptorSet descriptorSet;
  if (vkAllocateDescriptorSets(Vulkan::context(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  return DescriptorSet(descriptorSet);
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