/*
 * Server.h
 *
 *  Created on: Sep 25, 2017
 *      Author: sallomon1
 */

#ifndef SERVERUNIX_H_
#define SERVERUNIX_H_

#include <iostream>
#include <vector>
#include "FileOperations.h"

class ServerUnix
{
private:
  int _server_client;
public:
  ServerUnix(int _server_client);
  virtual ~ServerUnix();
  int ReceiveFile(int socket, char * name);
  int InitServerAndReceiveFile(std::string ip, int port, std::string file_list);
};

#endif /* SERVERUNIX_H_ */
