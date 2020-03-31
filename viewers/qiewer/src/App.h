#ifndef APP_H
#define APP_H

#include <QGUIApplication>

class OpenGLWindow;

class App : public QGuiApplication
{
  Q_OBJECT

public:
  explicit App(int& argc, char *argv[]);
  ~App();

protected:
  bool event(QEvent* e) override;

private:
  OpenGLWindow *const window;
};

#endif // APP_H
