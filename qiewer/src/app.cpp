#include <QDebug>
#include <QFileOpenEvent>

#include "app.h"
#include "window.h"

App::App(int& argc, char *argv[]) :
    QApplication(argc, argv), window(new Window())
{
    QCoreApplication::setOrganizationName("tobecodex");
    QCoreApplication::setOrganizationDomain("https://github.com/tobecodex");
    QCoreApplication::setApplicationName("qiewer");

    if (argc > 1) {
        window->load_stl(argv[1]);
    }

    window->show();
}

App::~App()
{
  delete window;
}

bool App::event(QEvent* e)
{
    if (e->type() == QEvent::FileOpen)
    {
        window->load_stl(static_cast<QFileOpenEvent*>(e)->file());
        return true;
    }
    else
    {
        return QApplication::event(e);
    }
}
