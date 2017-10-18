#include <iostream>
#include <vector>
#include "Log.h"
#include "FileOperations.h"
#include "ServerWin.h"

ServerWin::ServerWin(int server_client)
{
  _server_client = server_client;
  _tcpServer = new QTcpServer(this);
}

ServerWin::~ServerWin()
{
  delete _tcpServer;
}

int ServerWin::InitServerAndReceiveFile(std::string ip, int port, std::string file_list)
{
  Log( "InitServerAndReceiveFile START", normal);

  if(!_tcpServer->listen(QHostAddress::Any, 5555))
  {
      qDebug() << "Server could not start";
  }
  else
  {
      qDebug() << "Server started!";
  }

  qDebug() << "Server started!1";
  if (_tcpServer->hasPendingConnections())
  {
    _clientConnection = _tcpServer->nextPendingConnection();

    if(_clientConnection != NULL)
    {
      while (_clientConnection->bytesAvailable())
      {
          qDebug() << "----------------------------";
          QString temp = _clientConnection->readAll();
          qDebug() << temp;
          qDebug() << "----------------------------";
      }
    }
    else
      Log( "_clientConnection == NULL", normal);
  }
  else
  {
    Log( "_tcpServer has not pending connections", normal);
  }
  Log( "InitServerAndReceiveFile END", normal);
  return 0;
}

int ServerWin::ReceiveFile(int socket, char * name)
{
  Log( "ReceiveFile START", normal);

  Log( "ReceiveFile END", normal);
  return 0;
}
