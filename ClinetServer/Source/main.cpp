#include <string>
#include <iostream>
#include <fstream>
#include "Log.h"
#ifndef _WIN32
#include <unistd.h>
#include "ClientUnix.h"
#include "ServerUnix.h"
#define SLEEP(x) sleep(x)
#else

#include <windows.h>
#include "ClientWin.h"
#include "ServerWin.h"
#define SLEEP(x) Sleep(x*1000)

#endif
#include "FileOperations.h"


int main(int argc, char *argv[])
{
  Log( "Program started", normal);
  std::string IP;
  int PORT;
  std::vector<FileInfo> fi;
  FileOperations fo;
  std::string CS = argv[1];

  if(CS.compare("-c") == 0)
  {
    fo.ParseInputFile(argv[2]);
    IP = fo.GetIP();
    PORT = fo.GetPort();
    if (IP.length() <= 0)
    {
      Log( "IP address is not set in file " + (std::string)argv[2], error );
    }
    else
      Log( "IP: " + fo.GetIP(), normal);

    if (PORT == 0)
    {
      Log( "PORT is not set in file " + (std::string) argv[2], error );
    }
    else
      Log( "PORT: " + fo.GetPort(), normal);

    fi = fo.GetFiles();
#ifndef _WIN32
    ClientUnix cu(client);
#else
    ClientWin cu(client);
#endif
    for(int i = 0; (cu.InitClientAndSendFile(IP, PORT, argv[2],fi) != 0); i++)
      if(i > 30)
        break;
      else
        SLEEP(1);


#ifndef _WIN32
    ServerUnix su(client);
#else
    ServerWin su(client);
#endif

    for(int i = 0; (su.InitServerAndReceiveFile("", PORT, "filelist.txt") != 0); i++)
      if(i > 30)
        break;
      else
        SLEEP(1);

  }
  else
  {
    while(true)
    {
      fi.clear();

#ifndef _WIN32
      ServerUnix su(server);
#else
      ServerWin su(server);
#endif
      if(su.InitServerAndReceiveFile("", 5555, "filelist.txt") == 0)
      {
        SLEEP(3);

        fo.ParseInputFile("filelist.txt");
        IP = fo.GetRetIP();
        PORT = fo.GetPort();

        if (IP.length() <= 0)
        {
          Log( "IP address is not set in file filelist.txt", error);
        }
        else
          Log( "IP: " + fo.GetRetIP(), normal);

        if (PORT == 0)
        {
          Log( "PORT is not set in file filelist.txt", error);
        }
        else
          Log( "PORT: " + fo.GetPort(), normal);

        fi = fo.GetFiles();
#ifndef _WIN32
        ClientUnix cu(server);
#else
        ClientWin cu(server);
#endif
        for(int i = 0; (cu.InitClientAndSendFile(IP, PORT, "filelist.txt",fi) != 0); i++)
          if(i > 30)
            break;
          else
            SLEEP(1);
      }
      SLEEP(1);
    }
  }
  fflush(stdout);
  return 0;
}
