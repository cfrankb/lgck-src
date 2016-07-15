#include "helper.h"
#include "../shared/qtgui/cheat.h"
#include <QString>

const char *txt2wiki(QString & in)
{
    char v[in.length()+1];
    strcpy(v, q2c(in));
    char *s = v;
    char t[in.length()+1024];
    char *p = t;
    bool trim_s = true;
    bool code_b = false;
    while (*s) {
        char c = *s;
        if (c=='=' && s[1]=='{') {
            code_b = true;
            strcpy(p, "<code lua>");
            p += 10;
            s += 2;
            continue;
        }
        if (c=='=' && s[1]=='}') {
            code_b = false;
            strcpy(p, "</code>");
            p += 7;
            s += 2;
            continue;
        }
        ++s;
        if (c=='\n') {
            trim_s = true;
            if (code_b){
                *p++ = '\n';
              //  *p++ = ' ';
              //  *p++ = ' ';
                continue;
            }
        }
        if ((c == ' ' || c == '\t')
            && trim_s && !code_b) {
            continue;
        }
        trim_s = false;
        *p++ = c;
    }
    *p = 0;
    return t;
}
