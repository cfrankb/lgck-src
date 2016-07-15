
/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2011  Francois Blanchette

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

#include "stdafx.h"
#include <cstring>
#include <string>
#include "LuaVM.h"
#include "FileWrap.h"
#include "../shared/qtgui/cheat.h"

char *CLuaVM::m_debug = NULL;
int CLuaVM::m_debugLines = 0;
int CLuaVM::m_debugSize = 0;

void CLuaVM::registerInt(const char *name, int value)
{
    lua_pushnumber(m_luaState, value);
    lua_setglobal(m_luaState, name);
}

void CLuaVM::registerString(const char *name, const char *value)
{
    lua_pushstring(m_luaState, value);
    lua_setglobal(m_luaState, name);
}

void CLuaVM::registerBool(const char* name, bool value)
{
    lua_pushboolean(m_luaState, value);
    lua_setglobal(m_luaState, name);
}

void CLuaVM::reportErrors( int status )
{
    if ( status ) {
        const char *s =  lua_tostring(m_luaState, -1);
        char *t = new char[ strlen( s ) + 16 ];
        const char fmt [] = "-- [%d] %s\n";
        sprintf(t, fmt, status, s);
        debug(t);
        qDebug(fmt,status, s);
        delete []t;
        lua_pop(m_luaState, 1); // remove error message
    }
}

void CLuaVM::reportErrors( int status, const char *fnName )
{
    if ( status ) {
        const char *s =  lua_tostring(m_luaState, -1);
        char *t = new char[ strlen( s ) + strlen(fnName) + 16 ];
        const char fmt[] = "-- [%d] %s in %s\n";
        sprintf(t, fmt, status, s, fnName);
        debug(t);
        qDebug(fmt,status, s, fnName);
        delete []t;
        lua_pop(m_luaState, 1); // remove error message
    }
}

void CLuaVM::debug(const char *s)
{
    if (s) {
        addDebugString( s );
    }
    qDebug("%s", s);
}

/////////////////////////////////////////////////////////////////////////////
// lua interface

int CLuaVM::debug(lua_State *L)
{
    int argc = lua_gettop(L);
    for ( int n=1; n <= argc; ++n ) {
        const char *s = lua_tostring(L, n);
        if (s) {
            addDebugString( s );
        }
        qDebug("%s", s);
    }

    return 0;
}

#define MAX_LINES 30

void CLuaVM::addDebugLine(char *line, int size)
{
    if (m_debugLines == MAX_LINES) {
        strcpy(m_debug, strstr(m_debug, "\n") + 1);
        m_debugSize = strlen(m_debug);
        --m_debugLines;
    }

    if (m_debug) {
        char *t = new char [ m_debugSize + size + 2 ];
        strcpy(t, m_debug);
        strcat(t, "\n");
        strcat(t, line);
        m_debugSize += size + 1;
        delete [] m_debug;
        m_debug = t;

    } else {
        m_debug = new char [ size + 1];
        strcpy(m_debug, line);
        m_debugSize = size;
    }

    ++m_debugLines;
}

void CLuaVM::addDebugString(const char *s)
{
    char *t = new char[ strlen(s) + 1];
    strcpy(t, s);

    char *lineStart = t;
    char *lineEnd = strstr(lineStart, "\n");
    while (lineEnd) {
        if ((lineEnd != lineStart) && (lineEnd[-1] == '\r')) {
            lineEnd[-1] = 0;
        }
        *lineEnd = 0;
        addDebugLine( lineStart , strlen(lineStart));
        lineStart = lineEnd + 1;
        lineEnd = strstr(lineStart, "\n");
    }

    if (*lineStart) {
        addDebugLine(lineStart, strlen(lineStart));
    }

    delete [] t;
}

void CLuaVM::clearLog()
{
    if (m_debug) {
        delete [] m_debug;
        m_debug = NULL;
    }

    m_debug = new char[1];
    m_debug[0] = 0;
    m_debugSize = 0;
}


CLuaVM::CLuaVM()
{
    m_luaState = luaL_newstate();
    /* load Lua base libraries */
    luaL_openlibs(m_luaState);

    // register interface functions
    registerFn("ss_debug", debug);
    registerFn("debug", debug);
}

CLuaVM::~CLuaVM() {
    if (m_luaState) {
        m_luaState = NULL;
    }
}

char * CLuaVM::getDebugText() {
    return m_debug;
}

// lua interface

lua_State * CLuaVM::getState()
{
    return m_luaState;
}

int CLuaVM::exec(const char *luaCode)
{
    int status = luaL_dostring(m_luaState, luaCode);
    reportErrors( status );
    return status;
}

void CLuaVM::registerFn(const char * fnName, lua_CFunction fn)
{
    lua_register(m_luaState, fnName, fn);
}
