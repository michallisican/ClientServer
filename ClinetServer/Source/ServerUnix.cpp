/*
 * ServerUnix.cpp
 *
 *  Created on: Sep 25, 2017
 *      Author: sallomon1
 */
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include "Log.h"
#include "FileOperations.h"
#include "ServerUnix.h"

ServerUnix::ServerUnix(int server_client)
{
  _server_client = server_client;

}

ServerUnix::~ServerUnix()
{
  // TODO Auto-generated destructor stub
}

int ServerUnix::InitServerAndReceiveFile(std::string ip, int port, std::string file_list)
{
  Log( "InitServerAndReceiveFile START", normal);
  int socket_desc, new_socket, c = 0;
  struct sockaddr_in server, client;

  //Create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1)
  {
    Log("ERROR: Could not create socket", error);
    return 1;
  }

  int reuse = 1;
  if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof(reuse)) < 0)
  {
    Log("ERROR: setsockopt(SO_REUSEADDR) failed", error);
    return 1;
  }
  if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEPORT, (const char*) &reuse, sizeof(reuse)) < 0)
  {
    Log("ERROR: setsockopt(SO_REUSEPORT) failed", error);
    return 1;
  }
  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  //Bind
  if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0)
  {
    Log("ERROR: Bind failed", error);
    return 1;
  }

  Log("Bind done", normal);

  //Listen
  listen(socket_desc, 3);

  //Accept and incoming connection
  Log("Waiting for incoming connections...", normal);
  c = sizeof(struct sockaddr_in);

  int iResult;
  struct timeval tv;
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(socket_desc, &rfds);

  tv.tv_sec = (long)30;
  tv.tv_usec = 0;

  iResult = select(socket_desc + 1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);

  if(iResult > 0)
  {
    if ((new_socket = accept(socket_desc, (struct sockaddr *) &client, (socklen_t*) &c)))
    {
      close(socket_desc);
      Log("Connection accepted", normal);
    }
  }
  else
  {
    close(socket_desc);
    Log("Connection timeouted!", warning);
    return 1;
  }

  fflush(stdout);

  if (new_socket < 0)
  {
    Log("Accept Failed", error);
    return 1;
  }

  ReceiveFile(new_socket, (char *)file_list.c_str());

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
      while(ReceiveFile(new_socket,(char *)fn.c_str())==2)
      {
        //Log("Program started");
        Log( Log::ItoS(i) + " " + fn , warning);
        Log( "Read file size failed try it again", warning);
      }
      Log( "--------END: " + fn + "----------------" , special);
    }
  }

  close(new_socket);

  fflush(stdout);
  return 0;
}

int ServerUnix::ReceiveFile(int socket, char * name)
{
  int recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;

  char filearray[90241];
  FILE *file;

  do
  {
    Log("///////////////Read start\n", normal);
    int iResult;
    struct timeval tv;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(socket, &rfds);

    tv.tv_sec = (long)1;
    tv.tv_usec = 0;

    iResult = select(socket + 1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);

    if(iResult > 0)
    {
      stat = read(socket, &size, sizeof(int));
    }
    else
    {
      return 2;
    }
    Log("///////////////Read END\n", normal);
  } while (stat < 0);

  Log("Packet received.", normal);
  Log("Packet size: " + Log::ItoS(stat), normal);
  Log("file size: " + Log::ItoS(size), normal);

  if(size <= 0 || stat <= 0)
  {
    Log("File is empty\n", normal);
    return -1;
  }

  char buffer[] = "Got it";

  do
  {
    stat = write(socket, &buffer, sizeof(int));
  } while (stat < 0);

  Log("Reply sent\n", normal);

  file = fopen(name, "w");

  if (file == NULL)
  {
    Log("Error has occurred. File could not be opened", normal);
    return -1;
  }

  struct timeval timeout =
  { 10, 0 };

  fd_set fds;
  int buffer_fd;

  while (recv_size < size && packet_index <= size)
  {

    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

    if (buffer_fd < 0)
    {
      Log("error: bad file descriptor set.", error);
      return 1;
    }

    if (buffer_fd == 0)
    {
      Log("error: buffer read timeout expired.", normal);
      return 1;
    }

    if (buffer_fd > 0)
    {
      do
      {
        read_size = read(socket, filearray, 90241);
      } while (read_size < 0);

      Log("Packet number received: " + Log::ItoS(packet_index), normal);
      Log("Packet size: " + Log::ItoS(read_size), normal);

      if(packet_index <= 0 || read_size <= 0)
      {
        Log("End of file", normal);
        break;
      }

      write_size = fwrite(filearray, 1, read_size, file);
      Log("Written file size:  " + Log::ItoS(write_size), normal);

      if (read_size != write_size)
      {
        Log("ERROR in read write!!", error);
        return 1;
      }

      recv_size += read_size;
      packet_index++;
      Log("Total received file size: " + Log::ItoS(recv_size), normal);
    }

  }

  fclose(file);

  Log("File successfully Received!", normal);
  return 0;
}

