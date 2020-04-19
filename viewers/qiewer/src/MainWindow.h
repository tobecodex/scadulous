#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>

class Mesh;
class OpenGLWindow;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget * = NULL);

  void setMesh(Mesh *);

private:
  OpenGLWindow *m_glWindow;
};

#endif