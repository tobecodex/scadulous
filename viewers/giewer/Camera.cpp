#include "Camera.h"
#include "DescriptorPool.h"

Camera::Camera()
{
  
  _cameraBuffer = new ResourceBuffer(
    sizeof(_cameraMatrix), 
    VK_BUFFER_UNIFORM_BUFFER_BIT, 
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
}

Camera::~Camera()
{
  delete _descriptorPool;
}