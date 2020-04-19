#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "Mesh.h"
#include "Camera.h"

class QOpenGLDebugLogger;
class QOpenGLShaderProgram;

class OpenGLWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit OpenGLWindow(QWidget *parent);
  ~OpenGLWindow();

  virtual void setMesh(Mesh *);

protected:

  void keyPressEvent(QKeyEvent *) override;
  void keyReleaseEvent(QKeyEvent *);

  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;
  
private:
  int m_frames;

  Mesh *m_mesh;
  GLfloat *m_grid;
  Camera m_camera;

  GLuint m_posLoc;
  GLuint m_worldLoc, m_cameraLoc;
  QOpenGLShaderProgram *m_currentShader;
  QOpenGLVertexArrayObject m_vertexArrayObject;
  QOpenGLBuffer m_gridBuffer;

  void checkDebugLog();
  QOpenGLDebugLogger *m_debugLogger;
};