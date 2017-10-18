/*
 * ClientUnix.h
 *
 *  Created on: Sep 25, 2017
 *      Author: sallomon1
 */

#ifndef CLIENTUNIX_H_
#define CLIENTUNIX_H_

#include <iostream>
#include <vector>
#include "FileOperations.h"

class ClientUnix
{
private:
  int SendFile(int socket, char * name);
  int _server_client;
public:
  ClientUnix();
  ClientUnix(int server_client);
  virtual ~ClientUnix();
  int InitClientAndSendFile(std::string ip, int port, std::string file_list, std::vector<FileInfo> fi);
};

#endif /* CLIENTUNIX_H_ */
