/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "helper.h"
#include "../shared/qtgui/cheat.h"
#include <QString>

const char *txt2wiki(QString & in, char *out)
{
    char v[in.length()+1];
    strcpy(v, q2c(in));
    char *s = v;
    char *t = out;
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
