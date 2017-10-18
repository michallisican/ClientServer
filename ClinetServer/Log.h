/*
 * Log.h
 *
 *  Created on: Sep 27, 2017
 *      Author: sallomon1
 */

#ifndef _COLORS_
#define _COLORS_

/* FOREGROUND */
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define BOLD  "\x1B[1m"
#define UNDL  "\x1B[4m"

#endif  /* _COLORS_ */

#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <iostream>

enum{
  normal,
  warning,
  error,
  special,
};

class Log
{
public:
  Log(){}
  ~Log(){}
  static void Write(int line, std::string  file_name, std::string text, int state = normal)
  {
    std::string s = (std::string)(__DATE__) + ":" + (std::string)(__TIME__) + " [" + file_name.substr (3,file_name.length()) + ":" + ItoS(line) + "] " + text;
#ifndef _WIN32
    if(state == normal)
      std::cout << BOLD << KGRN << s << RST << RST << std::endl;
    else if(state == warning)
      std::cout << BOLD << KYEL << s << RST << RST << std::endl;
    else if(state == error)
      std::cout << BOLD << KRED << s << RST << RST << std::endl;
    else if(state == special)
      std::cout << BOLD << KMAG << s << RST << RST << std::endl;
    else
      std::cout << BOLD << KGRN << s << RST << RST << std::endl;
#else
    std::cout << s << std::endl;
#endif
  }

  static std::string ItoS(int i)
  {
#ifndef _WIN32
    char str[60];
    snprintf(str, 60, "%d", i);
    return str;
#else
    //std::cout << s << std::endl;
    return "";
#endif
  }
};

#define Log(str, stat) Log::Write(__LINE__, __FILE__, str, stat)


#endif /* LOG_H_ */
