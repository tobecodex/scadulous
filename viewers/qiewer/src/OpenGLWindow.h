#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

QT_BEGIN_NAMESPACE
class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;
class QOpenGLFunctions_4_3_Core; 
QT_END_NAMESPACE

#include "Mesh.h"

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
    void showEvent(QShowEvent *ev);

    bool event(QEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;

private:
    int m_frames;

    Mesh *m_mesh;
    QOpenGLContext *m_context;
    QOpenGLPaintDevice *m_device;

    GLuint m_posLoc;
    GLuint m_matrixLoc;
    QOpenGLShaderProgram *m_currentShader;
};