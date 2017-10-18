/*
 * FileOperations.h
 *
 *  Created on: Sep 25, 2017
 *      Author: sallomon1
 */

#ifndef FILEOPERATIONS_H_
#define FILEOPERATIONS_H_
#include <vector>

struct FileInfo
{
  std::string filename;
  int send_back;
  int size;
  FileInfo() :
    filename(""), send_back(0), size(0)
  {
  };
};

enum e_client_server{client, server};

class FileOperations
{
private:
  std::vector<FileInfo> _FileInfo;
  std::string _IP;
  std::string _RET_IP;
  int _PORT;
  int _server_client;
  std::string SearchingString(std::string key, std::string line);
public:
  FileOperations();
  FileOperations(int server_client);
  virtual ~FileOperations();
  std::string GetIP(){return _IP;}
  std::string GetRetIP(){return _RET_IP;}
  int GetPort(){return _PORT;}
  std::vector<FileInfo> GetFiles(){return _FileInfo;}
  int ParseInputFile(std::string file);
  std::string CurrentWorkingDirectory();
  int GetFileSize(std::string file);

};

#endif /* FILEOPERATIONS_H_ */
