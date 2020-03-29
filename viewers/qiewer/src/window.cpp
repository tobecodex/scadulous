#include <QtWidgets/QMenuBar>

#include "window.h"
#include "canvas.h"
#include "loader.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    open_action(new QAction("Open", this)),
    about_action(new QAction("About", this)),
    quit_action(new QAction("Quit", this))
{
    setWindowTitle("qiewer");

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(2, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(format);
    
    canvas = new Canvas(format, this);
    setCentralWidget(canvas);

    //QObject::connect(watcher, &QFileSystemWatcher::fileChanged,
    //                 this, &Window::on_watched_change);

    open_action->setShortcut(QKeySequence::Open);
    QObject::connect(open_action, &QAction::triggered,
                     this, &Window::on_open);

    quit_action->setShortcut(QKeySequence::Quit);
    QObject::connect(quit_action, &QAction::triggered,
                     this, &Window::close);

    /*reload_action->setShortcut(QKeySequence::Refresh);
    reload_action->setEnabled(false);
    QObject::connect(reload_action, &QAction::triggered,
                     this, &Window::on_reload);*/

    QObject::connect(about_action, &QAction::triggered,
                     this, &Window::on_about);

    auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(open_action);
    file_menu->addSeparator();
    file_menu->addAction(quit_action);

    resize(600, 400);
}

void Window::on_open()
{
  QString filename = QFileDialog::getOpenFileName(
              this, "Load .stl file", QString(), "*.stl");

  if (!filename.isNull()) {
      //load_stl(filename);
  }
}

void Window::on_about()
{
  QMessageBox::about(this, "", "<p align=\"center\"><b>qiewer</b></p>");
}

void Window::on_bad_stl()
{
  QMessageBox::critical(this, "Error", "");
}

void Window::on_empty_mesh()
{
  QMessageBox::critical(this, "Error", "<b>Error:</b><br>");
}

void Window::on_confusing_stl()
{
  QMessageBox::warning(this, "Warning", "<b>Warning:</b><br>");
}

void Window::on_missing_file()
{
    QMessageBox::critical(this, "Error", "<b>Missing file</b>");
}

bool Window::load_stl(const QString& filename)
{
    if (!open_action->isEnabled())  return false;

    canvas->set_status("Loading " + filename);

    Loader* loader = new Loader(this, filename);

    connect(loader, &Loader::finished,
            loader, &Loader::deleteLater);
    connect(loader, &Loader::finished,
            canvas, &Canvas::clear_status);

  if (filename[0] != ':')
  {
    /*(connect(
      loader, 
      &Loader::loaded_file,
      this, 
      &Window::setWindowTitle
    );*/
  }

    loader->start();
    return true;
}

/*void Window::keyPressEvent(QKeyEvent* event)
{
    if (!open_action->isEnabled())
    {
        QMainWindow::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Left)
    {
        load_prev();
        return;
    }
    else if (event->key() == Qt::Key_Right)
    {
        load_next();
        return;
    }

    QMainWindow::keyPressEvent(event);
}*/
