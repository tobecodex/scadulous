#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QVector3D>

QT_BEGIN_NAMESPACE
class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;
class QOpenGLFunctions_4_3_Core; 
QT_END_NAMESPACE

#include "Mesh.h"
#include "Camera.h"

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit OpenGLWindow(QWindow *parent = 0);
  ~OpenGLWindow();

  virtual void initialize();

  virtual void setMesh(Mesh *);

  virtual void render();
  virtual void render(QPainter *painter);

public slots:

protected:
  void showEvent(QShowEvent *);
  void resizeEvent(QResizeEvent *);

  bool event(QEvent *) override;
  void exposeEvent(QExposeEvent *) override;

  void keyPressEvent(QKeyEvent *);
  void keyReleaseEvent(QKeyEvent *);

private:
  int m_frames;

  Mesh *m_mesh;
  GLfloat *m_grid;

  QOpenGLContext *m_context;
  QOpenGLPaintDevice *m_device;

  GLuint m_posLoc;
  GLuint m_worldLoc, m_cameraLoc;
  QOpenGLShaderProgram *m_currentShader;

private:
  Camera m_camera;
};