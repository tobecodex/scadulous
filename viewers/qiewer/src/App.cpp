#include <QDebug>
#include <QFileOpenEvent>

#include "App.h"
#include "OpenGLWindow.h"

#include "Mesh.h"
#include "GLMesh.h"
#include "Loader.h"

App::App(int& argc, char *argv[]) : QGuiApplication(argc, argv), window(new OpenGLWindow())
{
  QCoreApplication::setOrganizationName("tobecodex");
  QCoreApplication::setOrganizationDomain("https://github.com/tobecodex");
  QCoreApplication::setApplicationName("qiewer");

  QSurfaceFormat format;
  format.setSamples(16);

  window->setFormat(format);
  window->resize(600, 400);
  window->show();

  Loader *loader = new Loader("../test_files/teapot.stl");
  Mesh *m = loader->load_stl();
  window->setMesh(m);
}

App::~App()
{
  delete window;
}

bool App::event(QEvent* e)
{
    if (e->type() == QEvent::FileOpen)
    {
        //window->load_stl(static_cast<QFileOpenEvent*>(e)->file());
        return true;
    }
    else
    {
        return QGuiApplication::event(e);
    }
}
