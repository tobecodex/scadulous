#include <QDebug>
#include "openglwindow.h"

#include <QtCore/QCoreApplication>

#include <QtGui/QPainter>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>

#include <QtWidgets/QMessageBox>

#include "Mesh.h"
#include "Loader.h"

static const char *vertexShaderSource =
    "#version 330 core\n"
    "in vec4 pos;\n"
    "uniform mat4 matrix;\n"
    "void main() {\n"
    "   gl_Position = matrix * pos;\n"
    "}\n";

/*"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\n";*/


static const char *fragmentShaderSource =
    "void main() {\n"
    "   gl_FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
    "}\n";

OpenGLWindow::OpenGLWindow(QWindow *parent) : QWindow(parent), m_context(0), m_device(0), m_mesh(0), m_frames(0)
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
  m_matrixLoc = m_currentShader->uniformLocation("matrix");
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

void OpenGLWindow::render()
{
  if (!isExposed() || !m_mesh)
    return;

  m_context->makeCurrent(this);

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

  glVertexAttribPointer(m_posLoc, 3, GL_FLOAT, GL_FALSE, 0, &m_mesh->vertices[0]);
 
  const qreal retinaScale = devicePixelRatio();
  glViewport(0, 0, width() * retinaScale, height() * retinaScale);

  glClear(GL_COLOR_BUFFER_BIT);

  m_currentShader->bind();

  QMatrix4x4 matrix;
  matrix.perspective(40.0f, (float)width() / (float)height(), 0.1f, 100.0f);
  matrix.translate(0, -2, -10);
  matrix.rotate(100.0f * m_frames / screen()->refreshRate(), 0, 1, 0);
  m_currentShader->setUniformValue(m_matrixLoc, matrix);

  glEnableVertexAttribArray(m_posLoc);

  glDrawArrays(GL_POINTS, 0, (int)m_mesh->vertices.size() / 3);

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