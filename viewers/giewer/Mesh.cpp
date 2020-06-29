#include "Mesh.h"

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
