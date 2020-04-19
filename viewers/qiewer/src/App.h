#ifndef APP_H
#define APP_H

#include <QApplication>

class MainWindow;
#include "SocketServer.h"

class App : public QApplication, public SocketClient
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
  MainWindow * _window;
  SocketServer *const _socketServer;
};

#endif // APP_H
