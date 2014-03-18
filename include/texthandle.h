#ifndef TEXTHANDLE_H
#define TEXTHANDLE_H

#include <string>


#define MAX_BUF 2048
std::string sjisToUTF8(const char *sjis);
std::string UTF8ToSJIS(const char *utf8);



#endif
