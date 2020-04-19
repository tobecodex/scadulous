#ifndef __CAMERA_H
#define __CAMERA_H

#include <QtGui/QOpenGLFunctions>

#include <QVector3D>
#include <QMatrix4x4>

class Camera
{
public:
  Camera();

  void lookAt(const QVector3D &);

  void forward(float);
  void backward(float);

  void right(float);
  void left(float);

  void up(float);
  void down(float);

  void position(const QVector3D &);
  void perspective(float vangle, float aspect, float near, float far);

  void pitch(float);
  void roll(float);
  void yaw(float);

  const QMatrix4x4 &matrix() const;

private:
  void update();

  QMatrix4x4 m_matrix;

  QVector3D m_position, m_up, m_ahead;
  float m_vangle, m_aspect, m_near, m_far;
};

#endif