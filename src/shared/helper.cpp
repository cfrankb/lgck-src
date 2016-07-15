#include <string.h>
#include <ctype.h>

const char *toUpper(char *s)
{
    for (unsigned int i=0; i < strlen(s);++i){
        if (isalpha(s[i])) {
            s[i] = toupper(s[i]);
        }
    }
    return s;
}
 
