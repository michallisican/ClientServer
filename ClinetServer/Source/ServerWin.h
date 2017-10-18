#ifndef SERVERWIN_H
#define SERVERWIN_H

#include <iostream>
#include <vector>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include "FileOperations.h"

class ServerWin : public QObject
{
  Q_OBJECT
private:
  int _server_client;
  QTcpServer *_tcpServer;
  QTcpSocket *_clientConnection;
  QNetworkSession *networkSession;
public:
  ServerWin(int server_client);
  virtual ~ServerWin();
  int ReceiveFile(std::string name);
  int InitServerAndReceiveFile(std::string ip, int port, std::string file_list);
};

#endif // SERVERWIN_H
