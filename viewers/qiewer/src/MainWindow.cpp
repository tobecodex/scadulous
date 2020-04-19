#include "MainWindow.h"
#include "OpenGLWindow.h"

#include <QstatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_glWindow(0)
{
  setCentralWidget(m_glWindow = new OpenGLWindow(this));
  centralWidget()->show();

  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setMesh(Mesh *m)
{
  m_glWindow->setMesh(m);
}