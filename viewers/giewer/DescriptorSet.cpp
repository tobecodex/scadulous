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

