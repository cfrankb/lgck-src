#include <string.h>
#include <ctype.h>
#include <cstdlib>
#include <cstdio>

const char *toUpper(char *s)
{
    for (unsigned int i=0; i < strlen(s);++i){
        if (isalpha(s[i])) {
            s[i] = toupper(s[i]);
        }
    }
    return s;
}
 
char *getUUID()
{
    char *uuid = new char[128];
    sprintf(uuid, "%.8x-%.4x-%.4x-%.4x-%.8x%.4x",
            rand(),
            rand() & 0xffff,
            rand() & 0xffff,
            rand() & 0xffff,
            rand(),
            rand() & 0xffff
            );
    return uuid;
}
