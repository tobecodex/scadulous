#include "Mesh.h"
#include "Vulkan.h"

#include <stdexcept>
#include "ResourceBuffer.h"

std::vector<VkVertexInputBindingDescription> &SimpleVertex::getVertexBindingDescriptions()
{
  static std::vector<VkVertexInputBindingDescription> bindingDescriptions { 
    { 0, sizeof(SimpleVertex), VK_VERTEX_INPUT_RATE_VERTEX }
  };
  return bindingDescriptions;
}


std::vector<VkVertexInputAttributeDescription> &SimpleVertex::getVertexAttributeDescriptions()
{
  static std::vector<VkVertexInputAttributeDescription> attributeDescriptions {
    { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SimpleVertex, _vertex) }
  };

  return attributeDescriptions;
}

std::vector<VkVertexInputBindingDescription> &LitVertex::getVertexBindingDescriptions()
{
  static std::vector<VkVertexInputBindingDescription> bindingDescriptions { 
    { 0, sizeof(LitVertex), VK_VERTEX_INPUT_RATE_VERTEX } 
  };
  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> &LitVertex::getVertexAttributeDescriptions()
{
  static std::vector<VkVertexInputAttributeDescription> attributeDescriptions {
    { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LitVertex, _vertex) },
    { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LitVertex, _normal) }
  };
  return attributeDescriptions;
}

Mesh::Mesh() : _transform(1.0f)
{
}

VkBuffer Mesh::vkBuffer() const
{
  return *_vertexBuffer;
}

SimpleMesh::SimpleMesh(const std::vector<glm::vec3> &vertices) : _vertices(vertices)
{
  /*_vertices.resize(vertices.size());
  for (int i = 0; i < vertices.size(); i++) {
    _vertices[i]._vertex = vertices[i];
  }*/
}

void SimpleMesh::createVertexBuffer()
{
  size_t bufSize = sizeof(SimpleVertex) * _vertices.size();

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

LitMesh::LitMesh(const std::vector<glm::vec3> &vertices)
{
  _vertices.resize(vertices.size());
  for (int i = 0; i < vertices.size(); i++) {
    _vertices[i]._vertex = vertices[i];
  }

  for (int i = 0; i < vertices.size(); i += 3) {
    glm::vec3 k = vertices[i + 1] - vertices[i];
    glm::vec3 l = vertices[i + 2] - vertices[i];
    glm::vec3 n = glm::normalize(glm::cross(k, l));
    _vertices[i]._normal = _vertices[i + 1]._normal = _vertices[i + 2]._normal = n;
  }
}

void LitMesh::createVertexBuffer()
{
  size_t bufSize = sizeof(LitVertex) * _vertices.size();

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

