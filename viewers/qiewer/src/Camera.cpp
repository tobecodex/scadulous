#include "Camera.h"

Camera::Camera() : m_position(0, 0, 0), m_ahead(0, 0, 1), m_up(0, 1, 0)
{
  perspective(90, 4.0f/3.0f, 0.1f, 100.0f);
  update();
}

void Camera::update()
{
  m_matrix.setToIdentity();
  //m_matrix.ortho(-5, +5, -5, +5, 0.1f, 100.0f);
  m_matrix.perspective(m_vangle, m_aspect, m_near, m_far);
  m_matrix.lookAt(m_position, m_position + m_ahead, m_up);
}

void Camera::position(const QVector3D &p)
{
  m_position = p;
}

void Camera::perspective(float vangle, float aspect, float _near, float _far)
{
  m_vangle = vangle;
  m_aspect = aspect;
  m_near = _near;
  m_far = _far;
  update();
}

const QMatrix4x4 &Camera::matrix() const
{
  return m_matrix;
}

void Camera::lookAt(const QVector3D &p)
{
  m_ahead = (p - m_position).normalized();
  m_up = QVector3D::normal(m_ahead, QVector3D::crossProduct(m_ahead, m_up));
  update();
}

void Camera::forward(float s)
{
  m_position += (m_ahead * s);
  update();
}

void Camera::backward(float s)
{
  forward(-s);
}

void Camera::right(float s)
{
  QVector3D right = QVector3D::normal(m_up, m_ahead);
  m_position += (right * -s);
  update();
}

void Camera::left(float s)
{
  right(-s);
}

void Camera::up(float s)
{
  m_position += (m_up * s);
  update();
}

void Camera::down(float s)
{
  up(-s);
}

void Camera::pitch(float theta)
{
  QMatrix4x4 t;
  t.setToIdentity();
  t.rotate(theta, QVector3D::normal(m_ahead, m_up));
  m_up = t * m_up;
  m_ahead = t * m_ahead;
  update();
}

void Camera::roll(float theta)
{
}

void Camera::yaw(float theta)
{
  QMatrix4x4 t;
  t.setToIdentity();
  t.rotate(theta, m_up);
  m_ahead = t * m_ahead;
  update();
}