#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QFileSystemWatcher>
#include <QCollator>

class Canvas;

class Window : public QMainWindow
{
  Q_OBJECT

public:
  explicit Window(QWidget* parent=0);

  bool load_stl(const QString &filename);

protected:

public slots:
  void on_open();
  void on_about();
  void on_bad_stl();
  void on_empty_mesh();
  void on_missing_file();
  void on_confusing_stl();

private slots:
  
private:
  QFileSystemWatcher *watcher;

  QAction* const open_action;
  QAction* const about_action;
  QAction* const quit_action;

  Canvas *canvas;
};

#endif // WINDOW_H
