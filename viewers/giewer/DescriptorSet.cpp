#include "DescriptorSet.h"
#include "ResourceBuffer.h"

DescriptorSet::DescriptorSet()
{
}

DescriptorSet::DescriptorSet(VkDescriptorSet &descriptorSet) : _descriptorSet(descriptorSet)
{
}

DescriptorSet::operator VkDescriptorSet()
{
  return _descriptorSet;
}

void DescriptorSet::bindResourceBuffer(ResourceBuffer &buffer, VkDescriptorType descriptorType) 
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
