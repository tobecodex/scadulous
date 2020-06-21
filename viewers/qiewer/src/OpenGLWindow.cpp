#include "OpenGLWindow.h"
#include <QtWidgets/QMessageBox>
#include <QOpenGLShaderProgram>
#include <QOpenGLDebugLogger>

#include <QKeyEvent>

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
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\n";

OpenGLWindow::OpenGLWindow(QWidget *parent) 
: QOpenGLWidget(parent), m_mesh(0), m_frames(0), m_debugLogger(0), m_gridBuffer(QOpenGLBuffer::VertexBuffer)
{
  setFocusPolicy(Qt::StrongFocus);

  m_camera.perspective(40.0f, (float)width() / (float)height(), 0.1f, 100.0f);
  m_camera.position(QVector3D(0, 2, -10));
  m_camera.lookAt(QVector3D(0, 0, 0));

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

  m_dirtyMesh = false;
}

OpenGLWindow::~OpenGLWindow()
{
}


void OpenGLWindow::checkDebugLog()
{
  const QList<QOpenGLDebugMessage> messages = m_debugLogger->loggedMessages();
  for (const QOpenGLDebugMessage &message : messages) {
    qDebug() << message;
  }
}

void OpenGLWindow::initializeGL()
{
  initializeOpenGLFunctions();

  // Setup a debug logger
  QOpenGLContext *currentContext = QOpenGLContext::currentContext();
  m_debugLogger = new QOpenGLDebugLogger(this);
  m_debugLogger->initialize();

  // Let's see what we've got..
  qDebug() << (const char *)glGetString(GL_VERSION);
  auto extensions = currentContext->extensions();
  for (auto i = extensions.begin(); i != extensions.end(); i++) {
    qDebug() << *i;
  }

  // Compile and link the shaders
  m_currentShader = new QOpenGLShaderProgram(this);
  if (!m_currentShader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
    qDebug() << m_currentShader->log();
  }
  if (!m_currentShader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
    qDebug() << m_currentShader->log();
  }
  if (!m_currentShader->link()) {
    qDebug() << m_currentShader->log();
  }

  // Grab the attribute locations
  m_posLoc = m_currentShader->attributeLocation("pos");
  m_worldLoc = m_currentShader->uniformLocation("world");
  m_cameraLoc = m_currentShader->uniformLocation("camera");

  // Create our (required) VAO
  m_vertexArrayObject.create();
  m_vertexArrayObject.bind();

  m_gridBuffer.create();
  m_gridBuffer.bind();
  checkDebugLog();
  m_gridBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  checkDebugLog();
  m_gridBuffer.allocate(m_grid, 60 * sizeof(float));
  checkDebugLog();
  
  m_currentShader->enableAttributeArray("pos");
  m_currentShader->setAttributeBuffer("pos", GL_FLOAT, 0, 3);
}

void OpenGLWindow::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT);

  QMatrix4x4 world;
  world.setToIdentity();

  m_currentShader->bind();
  //m_currentShader->setUniformValue(m_worldLoc, world);
  m_currentShader->setUniformValue(m_cameraLoc, m_camera.matrix());
  checkDebugLog();

  m_vertexArrayObject.bind();

  //m_gridBuffer.bind();
  //glDrawArrays(GL_LINES, 0, 20);

  if (m_dirtyMesh && m_mesh) {
    m_meshBuffer.create();
    m_meshBuffer.bind();
    m_meshBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_meshBuffer.allocate(&m_mesh->vertices[0], (int)m_mesh->vertices.size());
    m_dirtyMesh = false;
  }

  if (m_mesh) {
    m_meshBuffer.bind();
    glDrawArrays(GL_POINTS, 0, m_meshBuffer.size() / 3);
  }

  m_vertexArrayObject.release();
  m_currentShader->release();

  checkDebugLog();
}

void OpenGLWindow::resizeGL(int w, int h)
{
  m_camera.perspective(40.0f, (float)width() / (float)height(), 0.1f, 100.0f);
}

void OpenGLWindow::setMesh(Mesh *m)
{
  if (m_mesh) {
    delete m_mesh;
  }

  m_mesh = m;
  m_dirtyMesh = true;
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

  update();
}

void OpenGLWindow::keyReleaseEvent(QKeyEvent *event)
{
}