#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera(float width, float height) : _width(width), _height(height)
{
  _pos = { 0, 0, -1 };
  _ahead = { 0, 0, 1 };
  _up = { 0, 1, 0 };

  //_transform._model = glm::mat4(1.0f);
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

void Camera::forward(float s)
{
  _pos += (_ahead * s);
}

void Camera::backward(float s)
{
  _pos -= (_ahead * s);
}

void Camera::left(float s)
{
  _pos -= glm::normalize(glm::cross(_up, _ahead)) * s;
}

void Camera::right(float s)
{
  _pos += glm::normalize(glm::cross(_up, _ahead)) * s;
}

void Camera::up(float s)
{
  _pos += (_up * s);
}

void Camera::down(float s)
{
  _pos -= (_up * s);
}

void Camera::yaw(float theta)
{
  _ahead = glm::rotate(_ahead, theta, _up);
}

void Camera::pitch(float theta)
{
  glm::vec3 right = glm::normalize(glm::cross(_up, _ahead));
  _ahead = glm::rotate(_ahead, theta, right);
  _up = glm::cross(_ahead, right);
}

void Camera::roll(float theta)
{
  _up = glm::rotate(_up, theta, _ahead);
}

void Camera::lookAt(float x, float y, float z)
{
  _ahead = glm::normalize(glm::vec3(x,y,x) - _pos);
  //_up = glm::normalize(glm::cross(_ahead, _up));
}
