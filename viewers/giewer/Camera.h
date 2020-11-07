#ifndef __CAMERA_H
#define __CAMERA_H

#include "glm/glm.hpp"

#include "Vulkan.h"
#include "DescriptorPool.h"

class Camera 
{
private:
  struct { glm::mat4 model, view, proj; } _cameraMatrix;
  VertexBuffer _cameraBuffer;

  DescriptorPool *_descriptorPool = nullptr;

public:
  Camera();
  ~Camera();
};


#endif