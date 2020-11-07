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

class DescriptorSetLayout
{
public:
  VkDescriptorSetLayout _descriptorSetLayout;

public:
  DescriptorSetLayout(const Device &, uint32_t loc, VkDescriptorType type, VkShaderStageFlags shaderStages);
  ~DescriptorSetLayout();

  operator VkDescriptorSetLayout() const { return _descriptorSetLayout; }
};

class DescriptorSet
{
private:
  VkDescriptorSet _descriptorSet = nullptr;

public:
  DescriptorSet()
  {
  }

  DescriptorSet(VkDescriptorSet &descriptorSet) : _descriptorSet(descriptorSet)
  {
  }

  void DescriptorSet::bindResourceBuffer(ResourceBuffer &buffer, VkDescriptorType &descriptorType) 
  {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = buffer.size();

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = _descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = descriptorType;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr; // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional
    vkUpdateDescriptorSets(Vulkan::context(), 1, &descriptorWrite, 0, nullptr);
  }

  operator VkDescriptorSet() const { return _descriptorSet; }
};

#endif
