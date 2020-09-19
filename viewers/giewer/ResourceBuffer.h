#ifndef __BUFFER_H
#define __BUFFER_H

#include "Vulkan.h"

class ResourceBuffer
{
private:
  size_t _size;
  VkBuffer _buffer;
  VkDeviceMemory _bufferMemory;

public:
  ResourceBuffer(size_t size, VkBufferUsageFlagBits usageFlags, VkMemoryPropertyFlags memFlags);

  size_t size() const { return _size; }
  operator VkBuffer() const { return _buffer; }
  operator VkDeviceMemory() const { return _bufferMemory; }
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