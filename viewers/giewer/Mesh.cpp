#include "Mesh.h"
#include "Vulkan.h"

#include <stdexcept>
#include "ResourceBuffer.h"

Mesh::Mesh(const std::vector<glm::vec3> &vertices) : _vertices(vertices), _transform(1.0f)
{
}

VkBuffer Mesh::vkBuffer() const
{
  return (VkBuffer)*_vertexBuffer;
}

void Mesh::createVertexBuffer()
{
  size_t bufSize = sizeof(_vertices[0]) * _vertices.size();

  _vertexBuffer = new VertexBuffer(
    bufSize,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );

  void* data;
  if (vkMapMemory(Vulkan::ctx().device(), *_vertexBuffer, 0, bufSize, 0, &data) != VK_SUCCESS) {
    throw std::runtime_error("failed to map vertex buffer!");
  }
  memcpy(data, _vertices.data(), bufSize);
  vkUnmapMemory(Vulkan::ctx().device(), *_vertexBuffer);
}

VkVertexInputBindingDescription Mesh::getVertexBindingDescription()
{
  VkVertexInputBindingDescription bindingDescription;
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(glm::vec3);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescription;
}

VkVertexInputAttributeDescription Mesh::getVertexAttributeDescription() 
{
    VkVertexInputAttributeDescription attributeDescription;
    attributeDescription.binding = 0;
    attributeDescription.location = 0;
    attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription.offset = 0;
    return attributeDescription;
}
