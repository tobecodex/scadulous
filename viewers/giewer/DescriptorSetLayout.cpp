#include "DescriptorSetLayout.h"

#include "Device.h"

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