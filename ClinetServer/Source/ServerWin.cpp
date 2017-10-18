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
      Log( "Server could not start", error);
  else
      Log( "Server started!", normal);


  if (_tcpServer->waitForNewConnection(30000))
  {
      if(_tcpServer->hasPendingConnections())
      {
        Log( "Server has pending connections", normal);
        _clientConnection = _tcpServer->nextPendingConnection();
      }
      else
      {
          _clientConnection->close();
          _tcpServer->close();
          Log( "Server has not pending connections", warning);
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
       Log( "--------START: " + fn + " ----------------" , special);
       while(ReceiveFile(fn)==2)
       {
         //Log("Program started");
         Log( Log::ItoS(i) + " " + fn , warning);
         Log( "Read file size failed try it again", warning);
       }
       Log( "--------END: " + fn + " ----------------" , special);
     }
    }
    _clientConnection->close();
  }
  else
  {
    Log( "Server has not pending connections", warning);
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
    Log( "Client has connection", normal);

    char buffer[90241];
    _clientConnection->waitForReadyRead(10000);
    while (_clientConnection->bytesAvailable())
        readed = _clientConnection->read((char*) &size, sizeof(buffer));

    Log( "Readed from connection: " + Log::ItoS(readed), normal);
    Log( "File size: " + Log::ItoS(size), normal);

    if(size == 0 || readed == 0)
    {
      Log("File is empty\n", error);
      return -1;
    }

    _clientConnection->write("G");
    _clientConnection->waitForBytesWritten(1000);

    _clientConnection->waitForReadyRead(10000);

    QFile file( QString (name.c_str()));
    if (!file.open(QIODevice::WriteOnly))
    {
        Log( "Can not open file " + name, error);
        return -1;
    }
    if(!_clientConnection->bytesAvailable())
    {
        Log( "Bytes NOT Available", error);
        return -1;
    }

    int readed = 1;
    int recv_size = 0;
    while (recv_size < size && readed > 0)
    {
        _clientConnection->waitForReadyRead(10000);
        readed = _clientConnection->read(buffer, sizeof(buffer));
        Log( "Readed from connection: " + Log::ItoS(readed), normal);
        recv_size =+ readed;
        file.write(buffer,readed);
    }
    file.close();
  }
  else
    Log( "_clientConnection == NULL", error);

  Log( "ReceiveFile END", normal);
  return 0;
}
