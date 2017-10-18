/*
 * ClientUnix.cpp
 *
 *  Created on: Sep 25, 2017
 *      Author: sallomon1
 */
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Log.h"
#include "FileOperations.h"
#include "ClientUnix.h"

ClientUnix::ClientUnix()
{
  _server_client = 0;

}

ClientUnix::ClientUnix(int server_client)
{
  _server_client = server_client;
}

ClientUnix::~ClientUnix()
{
  // TODO Auto-generated destructor stub
}

int ClientUnix::InitClientAndSendFile(std::string ip, int port,
    std::string file_list, std::vector<FileInfo> fi)
{
  Log("InitClientAndSendFile START", normal);
  int socket_desc;
  struct sockaddr_in client;

  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc == -1)
  {
    Log("Could not create socket\n", error);
  }

  memset(&client, 0, sizeof(client));
  client.sin_addr.s_addr = inet_addr(ip.c_str());
  client.sin_family = AF_INET;
  client.sin_port = htons(port);

  if (connect(socket_desc, (struct sockaddr *) &client, sizeof(client)) < 0)
  {
    Log(strerror(errno), error);
    close(socket_desc);
    Log("Connect Error!!!", error);
    return 1;
  }

  Log("Connected\n", normal);

  if (_server_client != 1)
  {
    std::ofstream myf;
    myf.open(file_list.c_str(), std::ofstream::app);
    if (myf.is_open())
    {
      char *strAdd2 = inet_ntoa(client.sin_addr);
      myf << "\n[RET_IP]=" + (std::string) strAdd2 + "\n";
      myf.close();
    }
    else
    {
      Log( "Can not open " + file_list + " file", error);
      return 1;
    }
  }

  SendFile(socket_desc, (char *) file_list.c_str());
  sleep(1);

  for (unsigned int i = 0; i < fi.size(); i++)
    if (fi[i].size > 0)
      while(SendFile(socket_desc, (char *) fi[i].filename.c_str())==2)
      {
        Log( Log::ItoS(i) + " " + fi[i].filename , warning);
        Log( "Read file size failed try it again", warning);
      }

  close(socket_desc);
  return 0;
}

int ClientUnix::SendFile(int socket, char * name)
{
  FILE *file;
  int size, read_size, stat, packet_index;
  char send_buffer[90241], read_buffer[256];
  packet_index = 1;

  file = fopen(name, "r");
  Log("Getting File Size", normal);

  if (file == NULL)
  {
    Log("Error Opening file File", error);
  }

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);
  Log("Total File size: " + Log::ItoS(size), normal);
  if (size <= 0)
    return 0;

  //Send File Size
  Log("Sending File Size", normal);
  int ret = write(socket, (void *) &size, sizeof(int));


  //Send File as Byte Array
  Log("Sending File as Byte Array ret: " + Log::ItoS(ret), normal);

  do
  { //Read while we get errors that are due to signals.
    Log("Read start", normal);
    int iResult;
    struct timeval tv;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(socket, &rfds);

    tv.tv_sec = (long) 1;
    tv.tv_usec = 0;

    iResult = select(socket + 1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);

    if (iResult > 0)
    {
      stat = read(socket, &read_buffer, 255);
    }
    else
    {
      return 2;
    }
    Log("Bytes read: " + Log::ItoS(stat), normal);
  } while (stat < 0);

  Log("Received data in socket", normal);

  while (!feof(file))
  {
    //while(packet_index = 1){
    //Read from the file into our send buffer
    read_size = fread(send_buffer, 1, sizeof(send_buffer) - 1, file);

    //Send data through our socket
    do
    {
      stat = write(socket, send_buffer, read_size);
    } while (stat < 0);

    Log("Packet Number: " + Log::ItoS(packet_index), normal);
    Log("Packet Size Sent:" + Log::ItoS(read_size), normal);

    packet_index++;

    //Zero out our send buffer
    bzero(send_buffer, sizeof(send_buffer));
  }

  Log("Read file finished!!", normal);
  return 0;
}

