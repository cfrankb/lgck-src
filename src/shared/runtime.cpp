/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2016  Francois Blanchette

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

#include <cstring>
#include <cstdio>
#include <string>
#include "runtime.h"

void showUsage(const char *cmd)
{
    fprintf(stderr, "\nusage: %s [options] name_lgckdb\n", cmd);
    fprintf(stderr, "--skill=9              skill level 0 - 3\n");
    fprintf(stderr, "--level=99             start level (0 is level 1)\n");
    fprintf(stderr, "--size=9999x9999       window size\n");
    fprintf(stderr, "--help                 help screen\n\n");
}

enum {
    CMD_SKILL,
    CMD_LEVEL,
    CMD_SIZE,
    CMD_HELP,
    CMD_INVALID=-1
};

int whatIs(char *cmd)
{
    const char * cmds[] = {
        "skill",
        "level",
        "size",
        "help"
    };
    char tmp[strlen(cmd)+1];
    strcpy(tmp, cmd+2);
    char *p = strstr(tmp, "=");
    if (p) {
        *p = 0;
    }
    for (int i=0; i < sizeof(cmds)/sizeof(const char*); ++i) {
        if (!strcmp(tmp, cmds[i])){
            return i;
        }
    }
    return CMD_INVALID;
}

bool parseCmdLine(int argc, char* args[], ARGS & out)
{
    bool err = false;
    out.filename = "";
    out.skill = 0;
    out.level = -1;
    out.width = 640;
    out.height = 480;
    for (int i=1; !err && i < argc; ++i) {
        char tmp[strlen(args[i])+1];
        strcpy(tmp, args[i]);
        if (tmp[0]=='-') {
            if (tmp[1]!='-') {
                fprintf(stderr,"invalid command: %s\n", tmp);
                err = true;
                continue;
            }
            char *p = strstr(tmp, "=");
            switch (whatIs(tmp)) {
            case CMD_SKILL:
                if (!p || !p[1]) {
                    fprintf(stderr,"missing value: %s\n", tmp);
                    err = true;
                    break;
                }
                out.skill = atoi(p+1);
                break;
            case CMD_LEVEL:
                if (!p || !p[1]) {
                    fprintf(stderr,"missing value: %s\n", tmp);
                    err = true;
                    break;
                }
                out.level = atoi(p+1);
                break;
            case CMD_SIZE:
                if (!p || !p[1]) {
                    fprintf(stderr,"missing value: %s\n", tmp);
                    err = true;
                    break;
                }
                out.width = atoi(p+1);
                p = strstr(p, "x");
                if (p) {
                    out.height=atoi(p+1);
                } else {
                    fprintf(stderr,"missing height: %s\n", tmp);
                    err = true;
                }
                if (out.width < 50 || out.width > 16384) {
                    fprintf(stderr,"invalid width: %s\n", tmp);
                    err = true;
                } else if (out.height < 50 || out.height > 16384) {
                    fprintf(stderr,"invalid height: %s\n", tmp);
                    err = true;
                }
                break;
            case CMD_INVALID:
                fprintf(stderr,"invalid command: %s\n", tmp);
            case CMD_HELP:
                err = true;
                break;
            }
        } else {
            if (out.filename.empty()) {
                out.filename = tmp;
            }
        }
    }
    if (!err && out.filename.empty()) {
        err = true;
        fprintf(stderr,"no filename specified at command\n");
    }
    if (err) {
        showUsage(args[0]);
    }
    return !err;
}
