#include <QDebug>
#include "openglwindow.h"

#include <QtCore/QCoreApplication>

#include <QtGui/QPainter>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QKeyEvent>

#include <QtWidgets/QMessageBox>


#include "Mesh.h"
#include "Loader.h"

static const char *vertexShaderSource =
    "#version 330 core\n"
    "in vec4 pos;\n"
    //"uniform mat4 world;\n"
    "uniform mat4 camera;\n"
    "void main() {\n"
    "   gl_Position = camera * pos;\n"
    "}\n";

/*"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\n";*/


static const char *fragmentShaderSource =
    "void main() {\n"
    "   gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\n";

OpenGLWindow::OpenGLWindow(QWindow *parent) 
: QWindow(parent), m_context(0), m_device(0), m_mesh(0), m_frames(0)
{
  setSurfaceType(QWindow::OpenGLSurface);

  // Specify the format and create platform-specific surface
  QSurfaceFormat format;
  format.setDepthBufferSize( 24 );
  format.setMajorVersion( 4 );
  format.setMinorVersion( 3 );
  format.setSamples( 4 );
  format.setProfile( QSurfaceFormat::CoreProfile );
  setFormat( format );
  create();

  m_camera.position(QVector3D(0, -2, -10));
  m_camera.perspective(40.0f, (float)width() / (float)height(), 0.1f, 100.0f);

  float gridSize = 0.5f;
  int minX = -1, maxX = 1;
  int minZ = -1, maxZ = 1;

  int numLines = (((maxX - minX) / gridSize) + 1) + (((maxZ - minZ) / gridSize) + 1);

  m_grid = new GLfloat[numLines * 2 * 3];

  GLfloat *current = m_grid;
  for (GLfloat x = minX; x <= maxX; x += gridSize) {
    *current++ = x;
    *current++ = 0;
    *current++ = minZ;

    *current++ = x;
    *current++ = 0;
    *current++ = maxZ;
  }

  for (GLfloat z = minZ; z <= maxZ; z += gridSize) {
    *current++ = minX;
    *current++ = 0;
    *current++ = z;

    *current++ = maxX;
    *current++ = 0;
    *current++ = z;
  }
}

OpenGLWindow::~OpenGLWindow()
{
  delete m_device;
}

void OpenGLWindow::render(QPainter *painter)
{
  Q_UNUSED(painter);
}

void OpenGLWindow::initialize()
{
  m_currentShader = new QOpenGLShaderProgram(this);
  if (!m_currentShader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
    qDebug() << m_currentShader->log();
  }
  m_currentShader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
  m_currentShader->link();

  m_posLoc = m_currentShader->attributeLocation("pos");
  m_worldLoc = m_currentShader->uniformLocation("world");
  m_cameraLoc = m_currentShader->uniformLocation("camera");
}

void OpenGLWindow::setMesh(Mesh *m)
{
  if (m_mesh) {
    delete m_mesh;
  }

  m_mesh = m;
  QMetaObject::invokeMethod(this, [this]{ this->requestUpdate(); });
}

void OpenGLWindow::showEvent(QShowEvent *ev)
{
  if (!m_context) {
    m_context = new QOpenGLContext(this);
    m_context->setFormat(requestedFormat());
    m_context->create();
  
    m_context->makeCurrent(this);
    
    initializeOpenGLFunctions();
    initialize();
  }
}

void OpenGLWindow::resizeEvent(QResizeEvent *ev)
{
  m_camera.perspective(40.0f, (float)width() / (float)height(), 0.1f, 100.0f);
}

static void cube() {

  GLfloat vertices[] = {
    -1.0f, 0.707f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f
  };

  const GLfloat cube[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end

    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end

    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,

    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,

    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,

    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,

    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
  };
}

void OpenGLWindow::render()
{
  if (!isExposed())
    return;

  m_context->makeCurrent(this);

 
  const qreal retinaScale = devicePixelRatio();
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);

  glClear(GL_COLOR_BUFFER_BIT);

  QMatrix4x4 world;
  world.setToIdentity();

  m_currentShader->bind();
  //m_currentShader->setUniformValue(m_worldLoc, world);
  m_currentShader->setUniformValue(m_cameraLoc, m_camera.matrix());

  if (m_mesh) {
    glVertexAttribPointer(m_posLoc, 3, GL_FLOAT, GL_FALSE, 0, &m_mesh->vertices[0]);

    glEnableVertexAttribArray(m_posLoc);
    glDrawArrays(GL_POINTS, 0, (int)m_mesh->vertices.size() / 3);
    glDisableVertexAttribArray(m_posLoc);
  }
  
  glVertexAttribPointer(m_posLoc, 3, GL_FLOAT, GL_FALSE, 0, m_grid);

  glEnableVertexAttribArray(m_posLoc);
  glDrawArrays(GL_LINES, 0, 20);
  glDisableVertexAttribArray(m_posLoc);

  m_currentShader->release();

  m_context->swapBuffers(this);
  m_frames++;
  
  requestUpdate();
}

bool OpenGLWindow::event(QEvent *event)
{
  switch (event->type()) {
  case QEvent::UpdateRequest:
    render();
    return true;
  default:
    return QWindow::event(event);
  }
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
  Q_UNUSED(event);

  if (isExposed())
    render();
}

void OpenGLWindow::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {

    case Qt::Key_W : {
      m_camera.forward(1);
    }
    break;
    
    case Qt::Key_S : {
      m_camera.backward(1);
    }
    break;
    
    case Qt::Key_A : {
      m_camera.left(1);
    }
    break;
    
    case Qt::Key_D : {
      m_camera.right(1);
    }
    break;
    
    case Qt::Key_Q : {
      m_camera.up(1);
    }
    break;
    
    case Qt::Key_Z : {
      m_camera.down(1);
    }
    break;

    case Qt::Key_Up : {
      m_camera.pitch(1);
    }
    break;

    case Qt::Key_Down : {
      m_camera.pitch(-1);
    }
    break;

    case Qt::Key_Left : {
      m_camera.yaw(-1);
    }
    break;

    case Qt::Key_Right : {
      m_camera.yaw(1);
    }
    break;

    case Qt::Key_Space : {
      m_camera.lookAt(QVector3D(0, 0, 0));
    }
    break;
  }
}

void OpenGLWindow::keyReleaseEvent(QKeyEvent *event)
{
}