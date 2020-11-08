#include "SocketServer.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

SocketServer::SocketServer(short port) : 
  _port(port),
  _acceptor(
      _io_context, 
      boost::asio::ip::tcp::endpoint(tcp::v4(), port)
  )
{
}

SocketServer::~SocketServer()
{
  if (_serverThread) {
    delete _serverThread;
  }
}

void SocketServer::start(SocketClient &client)
{
  _client = &client;
  acceptNext();
}

void SocketServer::acceptNext()
{
  //tcp::socket *socket = new tcp::socket(_io_context);
  _acceptor.async_accept(
    [this](boost::system::error_code err, tcp::socket s) {
      if (!err) {
        this->_client->onConnect(tcp::iostream(std::move(s)));
        this->acceptNext();
      }
    }
  );
  _serverThread = new std::thread(&boost::asio::io_context::run, &_io_context);
}