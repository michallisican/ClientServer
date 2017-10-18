#include <iostream>
#include <vector>
#include "Log.h"
#include "FileOperations.h"
#include <QSignalMapper>
#include "ServerWin.h"
#include <windows.h>
#include <QFile>

ServerWin::ServerWin(int server_client)
{
  _server_client = server_client;
  _tcpServer = new QTcpServer(this);
  _clientConnection = NULL;
}

ServerWin::~ServerWin()
{
  delete _tcpServer;
}

int ServerWin::InitServerAndReceiveFile(std::string ip, int port, std::string file_list)
{
  Log( "InitServerAndReceiveFile START", normal);

  if(!_tcpServer->listen(QHostAddress::Any, port))
  {
      qDebug() << "Server could not start";
  }
  else
  {
      qDebug() << "Server started!";
  }

  if (_tcpServer->waitForNewConnection(10000))//_tcpServer->hasPendingConnections())
  {
      if(_tcpServer->hasPendingConnections())
      {
        Log( "_tcpServer->hasPendingConnections", normal);
        _clientConnection = _tcpServer->nextPendingConnection();
      }
      else
      {
          _clientConnection->close();
          _tcpServer->close();
          Log( "Server has not PendingConnections", normal);
          return -1;
      }

     ReceiveFile(file_list);

     FileOperations fo;
     fo.ParseInputFile(file_list);
     std::string fn = "";

     std::vector<FileInfo> fi = fo.GetFiles();

    for (unsigned int i = 0; i < fi.size(); i++)
    {
     fn = fi[i].filename;
     if(_server_client == 0)
     {
       if(fn.length() > 0)
         fn += "_out";
     }
     if(fn.length() > 0)
     {
       Log( "--------START: " + fn + "----------------" , special);
       while(ReceiveFile(fn)==2)
       {
         //Log("Program started");
         //Log( Log::ItoS(i) + " " + fn , warning);
         Log( "Read file size failed try it again", warning);
       }
       Log( "--------END: " + fn + "----------------" , special);
     }
    }
    _clientConnection->close();
  }
  else
  {
    Log( "_tcpServer has not pending connections", normal);
    _tcpServer->close();
    return -1;
  }

  _tcpServer->close();
  Log( "InitServerAndReceiveFile END", normal);
  return 0;
}

int ServerWin::ReceiveFile(std::string name)
{
  Log( "ReceiveFile START", normal);
  int size = 0;
  int readed = 0;
  if(_clientConnection != NULL)
  {
    Log( "_clientConnection != NULL", normal);

    char buffer[90240];
    _clientConnection->waitForReadyRead(10000);
    while (_clientConnection->bytesAvailable())
        readed = _clientConnection->read((char*) &size, sizeof(buffer));
    qDebug() << "size " << size;
    qDebug() << "readed " << readed;
    if(size == 0 || readed == 0)
    {
      Log("File is empty\n", normal);
      return -1;
    }

    _clientConnection->write("Got1");
    _clientConnection->waitForBytesWritten(1000);

    _clientConnection->waitForReadyRead(10000);
    QFile file( QString (name.c_str()));
    if (!file.open(QIODevice::WriteOnly))
        return -1;

    while (_clientConnection->bytesAvailable())
    {
        int readed = _clientConnection->read(buffer, sizeof(buffer));
        file.write(buffer,readed);
    }
    file.close();
  }
  else
    Log( "_clientConnection == NULL", normal);

  Log( "ReceiveFile END", normal);
  return 0;
}
