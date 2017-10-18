#ifndef CLIENTWIN_H
#define CLIENTWIN_H

#include <iostream>
#include <vector>
#include <QObject>
#include <QTcpSocket>
#include "FileOperations.h"

class ClientWin : public QObject
{
  Q_OBJECT
private:
  int SendFile(std::string name);
  int _server_client;
  QTcpSocket *socket;
public:
  ClientWin();
  ClientWin(int server_client);
  virtual ~ClientWin(){}
  int InitClientAndSendFile(std::string ip, int port, std::string file_list, std::vector<FileInfo> fi);
};

#endif // CLIENTWIN_H
