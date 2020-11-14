#ifndef __MESH_H
#define __MESH_H

#include <vector>
#include <glm/glm.hpp>
#include "vulkan/vulkan.h"

class VertexBuffer;

class SimpleVertex
{
public:
  glm::vec3 _vertex;
  static std::vector<VkVertexInputBindingDescription> &getVertexBindingDescriptions();
  static std::vector<VkVertexInputAttributeDescription> &getVertexAttributeDescriptions();
};

class LitVertex : public SimpleVertex
{
public:
  glm::vec3 _normal;
  static std::vector<VkVertexInputBindingDescription> &getVertexBindingDescriptions();
  static std::vector<VkVertexInputAttributeDescription> &getVertexAttributeDescriptions();
};

class Mesh
{
protected:
  Mesh();
  glm::mat4 _transform;
  VertexBuffer *_vertexBuffer = nullptr;

public:
  void transform(glm::mat4 &t) { _transform = t; }
  const glm::mat4 &transform() const { return _transform; }
  
  virtual uint32_t count() = 0;
  virtual void createVertexBuffer() = 0;

  VkBuffer vkBuffer() const;
};

class SimpleMesh : public Mesh
{
private:
  std::vector<glm::vec3> _vertices;

public:
  SimpleMesh(const std::vector<glm::vec3> &vertices);
  virtual uint32_t count() { return (uint32_t)_vertices.size(); }
  virtual void createVertexBuffer();
};

class LitMesh : public Mesh
{
private:
  std::vector<LitVertex> _vertices;

public:
  LitMesh(const std::vector<glm::vec3> &vertices);
  virtual uint32_t count() { return (uint32_t)_vertices.size(); }
  virtual void createVertexBuffer();
};

#endif