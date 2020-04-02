#ifndef APP_H
#define APP_H

#include <QGUIApplication>
#include "SocketServer.h"

class OpenGLWindow;

class App : public QGuiApplication, public SocketClient
{
  Q_OBJECT

public:
  explicit App(int& argc, char *argv[]);
  ~App();

  void onConnect(std::iostream &);
  void onClose(std::iostream &);

protected:
  bool event(QEvent* e) override;

private:
  OpenGLWindow *const _window;
  SocketServer *_socketServer;
};

#endif // APP_H
