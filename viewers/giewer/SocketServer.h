#ifndef __SOCKET_SERVER_H
#define __SOCKET_SERVER_H

#include <thread>
#include <iostream>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

class SocketClient
{
public:
  virtual void onConnect(std::iostream &) = 0;
  virtual void onClose(std::iostream &) = 0;
};

class SocketServer
{
public:
  SocketServer(short port = 4242);
  ~SocketServer();

  void start(SocketClient &);

protected:
  short _port;
  SocketClient *_client;
  std::thread *_serverThread = nullptr;
  boost::asio::io_context _io_context;
  tcp::acceptor _acceptor;

  void acceptNext();
  void handleConnect(const boost::system::error_code &, tcp::socket &);
};

#endif