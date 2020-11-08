#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(float width, float height) : _width(width), _height(height)
{
  _pos = { 0, 0, -1 };
  _ahead = { 0, 0, 1 };
  _up = { 0, 1, 0 };

  _transform._model = glm::mat4(1.0f);
  _transform._proj = glm::perspective(glm::radians(45.0f), _width / (float)_height, 0.1f, 10.0f);
  _transform._proj[1][1] *= -1;
}

Camera::~Camera()
{
}

const ViewTransform &Camera::transform()
{
  _transform._view = glm::lookAt(_pos, _pos + _ahead, _up);
  return _transform;
}

void Camera::forward()
{
  _pos += (_ahead * 0.1f);
}

void Camera::backward()
{
  _pos -= (_ahead * 0.1f);
}

void Camera::left()
{
  _pos -= glm::normalize(glm::cross(_up, _ahead)) * 0.1f;
}

void Camera::right()
{
  _pos += glm::normalize(glm::cross(_up, _ahead)) * 0.1f;
}

void Camera::lookAt(float x, float y, float z)
{
  _ahead = glm::normalize(glm::vec3(x,y,x) - _pos);
  //_up = glm::normalize(glm::cross(_ahead, _up));
}
