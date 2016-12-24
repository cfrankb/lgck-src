#ifndef __HELPER_H__LGCK
#define __HELPER_H__LGCK
#include <string>
#include <list>
const char *toUpper(char *s); 
char *getUUID();
bool copyFile(const std::string in, const std::string out, std::string errMsg);
bool concat(const std::list<std::string> files, std::string out, std::string msg);
#ifdef _WIN32
    char *realpath(const char *path, char resolved_path[PATH_MAX]);
#else
    #include <stdlib.h>
    #include <linux/limits.h>
#endif

#endif
