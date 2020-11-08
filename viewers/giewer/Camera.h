#ifndef __CAMERA_H
#define __CAMERA_H

#include "glm/glm.hpp"

typedef struct { glm::mat4 _model, _view, _proj; } ViewTransform;

class Camera
{
private:
  float _width, _height;
  ViewTransform _transform;
  glm::vec3 _pos, _ahead, _up;

public:
  Camera(float width, float height);
  ~Camera();

  void forward();
  void backward();
  void left();
  void right();
  void lookAt(float x, float y, float z);

  const ViewTransform &transform();
};

#endif