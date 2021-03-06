#include "ResourceBuffer.h"
#include "Device.h"
#include "Vulkan.h"

#include <stdexcept>

static uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags memFlags)
{
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & memFlags) == memFlags) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

ResourceBuffer::ResourceBuffer(size_t size, VkBufferUsageFlagBits usageFlags, VkMemoryPropertyFlags memFlags) 
: _size(size)
{
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usageFlags;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(Vulkan::ctx().device(), &bufferInfo, nullptr, &_buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(Vulkan::ctx().device(), _buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(
    Vulkan::ctx().physicalDevice(), memRequirements.memoryTypeBits, memFlags
  );

  if (vkAllocateMemory(Vulkan::ctx().device(), &allocInfo, nullptr, &_bufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }

  if (vkBindBufferMemory(Vulkan::ctx().device(), _buffer, _bufferMemory, 0) != VK_SUCCESS) {
    throw std::runtime_error("failed to bind buffer memory!");
  }
}

ResourceBuffer::operator VkBuffer()
{
  return _buffer;
}

ResourceBuffer::operator VkDeviceMemory()
{
  return _bufferMemory;
}
