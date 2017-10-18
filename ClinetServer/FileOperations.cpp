/*
 * FileOperations.cpp
 *
 *  Created on: Sep 25, 2017
 *      Author: sallomon1
 */
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd // stupid MSFT "deprecation" warning
#else
#include <unistd.h>
#endif
#include "Log.h"
#include "FileOperations.h"

FileOperations::FileOperations()
{
  //_FileInfo.clear();
  _server_client = 0;
  _IP = _RET_IP = "";
  _PORT = 0;
  _FileInfo.clear();

}

FileOperations::FileOperations(int server_client)
{
  //_FileInfo.clear();
  _server_client = server_client;
  _IP = _RET_IP = "";
  _PORT = 0;
  _FileInfo.clear();

}

FileOperations::~FileOperations()
{
  //_FileInfo.clear();
  _IP = _RET_IP = "";
  _PORT = 0;
  _FileInfo.clear();
}

std::string FileOperations::SearchingString(std::string key, std::string line)
{
  std::string value = "";

  std::size_t found = line.find(key);
  if (found!=std::string::npos)
    value = line.substr(found+ key.length() + 1, line.length());

  return value;
}

int FileOperations::ParseInputFile(std::string file)
{
  _FileInfo.clear();
  std::string line;
  std::string current_dir_and_file = CurrentWorkingDirectory() + "/";
  current_dir_and_file.append(file);
  Log(current_dir_and_file, special);
  GetFileSize(current_dir_and_file);
  std::ifstream myfile(current_dir_and_file.c_str());
  if (myfile.is_open())
  {
    while (getline(myfile, line))
    {
      std::string tmp = "";
      Log(line,special);
      tmp = SearchingString("[IP]", line);
      if(tmp.length() > 0)
        _IP = tmp;

      tmp = SearchingString("[RET_IP]", line);
      if(tmp.length() > 0)
        _RET_IP = tmp;

      tmp = SearchingString("[PORT]", line);
      if(tmp.length() > 0)
      {
        std::stringstream convert(tmp);
        if ( !(convert >> _PORT) )
          _PORT = 0;
      }

      tmp = SearchingString("[FILE]", line);
      if(!tmp.empty() && ((GetFileSize(tmp) > 0) || (_server_client = 1)))
      {
        FileInfo fi;
        if(tmp[tmp.length()] == '\r')
          tmp = tmp.substr(0, tmp.length()-1);

        fi.filename = tmp;
        fi.send_back = 1;
        fi.size = GetFileSize(tmp);
        _FileInfo.push_back(fi);
      }
    }
    myfile.close();
  }
  else
  {
    Log("Can not open file", normal);
    return -1;
  }
  return 0;
}

std::string FileOperations::CurrentWorkingDirectory()
{
  char buffer[256];
  char *answer = getcwd(buffer, sizeof(buffer));
  std::string s_cwd = "";
  if (answer)
    s_cwd = answer;

  return s_cwd;
}

int FileOperations::GetFileSize(std::string file)
{
  std::streampos begin, end;
  std::ifstream myfile(file.c_str());
  begin = myfile.tellg();
  myfile.seekg(0, std::ios::end);
  end = myfile.tellg();
  myfile.close();
  return end - begin;
}

