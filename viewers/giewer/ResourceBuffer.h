#ifndef __RESOURCE_BUFFER_H
#define __RESOURCE_BUFFER_H

#include "vulkan/vulkan.h"

class ResourceBuffer
{
private:
  size_t _size;
  VkBuffer _buffer;
  VkDeviceMemory _bufferMemory;

public:
  ResourceBuffer(ResourceBuffer &&other) = default;
  ResourceBuffer &operator =(ResourceBuffer &&other) = default;
  ResourceBuffer(size_t size, VkBufferUsageFlagBits usageFlags, VkMemoryPropertyFlags memFlags);

  size_t size() const { return _size; }

  //operator VkBuffer() const { return _buffer; }
  operator VkDeviceMemory() const { return _bufferMemory; }

  //VkBuffer VkBuffer() { return _buffer; }
  //VkDeviceMemory VkDeviceMemory() { return _bufferMemory; }
};

class VertexBuffer : public ResourceBuffer
{
public:
  VertexBuffer(size_t size, VkMemoryPropertyFlags memFlags)
  : ResourceBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memFlags) {}
};

class UniformBuffer : public ResourceBuffer
{
public:
  UniformBuffer(size_t size, VkMemoryPropertyFlags memFlags)
  : ResourceBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, memFlags) {}
};

#endif