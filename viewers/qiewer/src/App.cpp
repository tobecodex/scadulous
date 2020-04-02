#include <QDebug>
#include <QFileOpenEvent>

#include "App.h"
#include "OpenGLWindow.h"

#include "Mesh.h"
#include "Loader.h"

#include <iostream>

App::App(int& argc, char *argv[]) : QGuiApplication(argc, argv), _window(new OpenGLWindow()), _socketServer(0)
{
  QCoreApplication::setOrganizationName("tobecodex");
  QCoreApplication::setOrganizationDomain("https://github.com/tobecodex");
  QCoreApplication::setApplicationName("qiewer");

  QSurfaceFormat format;
  format.setSamples(16);

  _window->setFormat(format);
  _window->resize(600, 400);
  _window->show();

  _socketServer = new SocketServer();
  _socketServer->start(*this);
}

App::~App()
{
  delete _window;
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

void App::onConnect(std::iostream &s)
{
  Loader loader;
  Mesh *m = loader.load_stl(s);
}

void App::onClose(std::iostream &s)
{
}