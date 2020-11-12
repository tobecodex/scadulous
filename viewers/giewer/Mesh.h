#ifndef __MESH_H
#define __MESH_H

#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "ResourceBuffer.h"

class Mesh
{
private:
  glm::mat4 _transform;
  std::vector<glm::vec3> _vertices;
  VertexBuffer *_vertexBuffer = nullptr;

public:
  Mesh(const std::vector<glm::vec3> &vertices);

  void transform(glm::mat4 &t) { _transform = t; }
  const glm::mat4 &transform() const { return _transform; }
  const std::vector<glm::vec3> &vertices() const { return _vertices; }

  static VkVertexInputBindingDescription getVertexBindingDescription();
  static VkVertexInputAttributeDescription getVertexAttributeDescription();
  
  void createVertexBuffer();
  VkBuffer vkBuffer() const;
};

#endif