#ifndef __MESH_H
#define __MESH_H

#include <vector>

#include <glm/vec3.hpp>
#include <vulkan/vulkan.h>

class Mesh
{
private:
  std::vector<glm::vec3> _vertices;

public:
  Mesh(std::vector<glm::vec3> &vertices) : _vertices(vertices)
  {
  }

  const std::vector<glm::vec3> &vertices() const { return _vertices; }

  static VkVertexInputBindingDescription getVertexBindingDescription();
  static VkVertexInputAttributeDescription getVertexAttributeDescription();
};

#endif