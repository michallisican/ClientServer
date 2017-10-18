#include <iostream>
#include <vector>
#include <QDataStream>
#include <QLocalSocket>
#include <QFile>
#include "FileOperations.h"
#include "ClientWin.h"
#include <windows.h>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QNetworkInterface>
#include "Log.h"

ClientWin::ClientWin()
{
  socket = new QTcpSocket(this);
}

ClientWin::ClientWin(int server_client)
{
  _server_client = server_client;
  socket = new QTcpSocket(this);
}

int ClientWin::InitClientAndSendFile(std::string ip, int port, std::string file_list, std::vector<FileInfo> fi)
{
  QString local_ip = "";
  foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
      if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
      {
        Log("IP: " + address.toString().toStdString(), normal);
        local_ip = address.toString();
        break;
      }
  }

  socket->connectToHost(QString::fromStdString(ip), port);

  if(socket->waitForConnected(1000))
  {
    FileOperations fo;
    Log("Connected!", normal);
    QFile file(QString::fromStdString(fo.CurrentWorkingDirectory() + "\\" + file_list));   //file path
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
      QTextStream stream(&file);
      stream << "[RET_IP]=" << local_ip << endl;
    }
    file.close();


    SendFile(file_list);

    Sleep(1000);
    for (unsigned int i = 0; i < fi.size(); i++)
    {
      if(fi[i].size > 0)
      {
        std::cout << fi[i].size << " " << fi[i].filename << std::endl;
        SendFile((char *)fi[i].filename.c_str());
      }
    }
    socket->close();
  }
  else
  {
      Log("Not Connected!", warning);
      //delete socket;
      return -1;
  }
  //delete socket;
  return 0;
}

int ClientWin::SendFile(std::string file_name)
{
  char buffer[90240];
  FileOperations fo;
  int file_size = fo.GetFileSize(fo.CurrentWorkingDirectory() + "\\" + file_name);

  Log("Total File size: " + Log::ItoS(file_size), warning);
  if(file_size <= 0)
    return 0;

  socket->write((const char*) &file_size, sizeof(file_size));
  socket->waitForBytesWritten(1000);

  if(socket->bytesAvailable())
  {
    QByteArray b = socket->readAll();
    //qDebug(b);
  }

  QFile file(QString::fromStdString(file_name));   //file path
  file.open(QIODevice::ReadOnly);
  while (!file.atEnd())
  {
    int readed = file.read(buffer, sizeof(buffer));
    Log("Writed: " + Log::ItoS(readed), warning);
    socket->write(buffer, readed);
    while(socket->waitForBytesWritten(1000))
    {
      Sleep(10);
    }
  }

  Sleep(1000);



  return 0;
}

