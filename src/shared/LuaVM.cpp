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

#include "compat/asprintf.h"
#include "stdafx.h"
#include <cstring>
#include <string>
#include <stdio.h>
#include "LuaVM.h"
#include "FileWrap.h"

std::function<void(const char *)> CLuaVM::m_callback = nullptr;

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

void CLuaVM::reportErrors(int status, const char *fnName)
{
    if (status) {
        const char *s =  lua_tostring(m_luaState, -1);
        char *t = nullptr;
        const char *fmt = fnName ? "-- [%d] %s in %s\n" : "-- [%d] %s\n";
        if (asprintf(&t, fmt, status, s, fnName) == -1) {
            qDebug("allocation error with asprintf().");
        } else {
            debug(t);
            free(t);
        }
        qDebug(fmt, status, s, fnName);
        lua_pop(m_luaState, 1); // remove error message
    }
}

void CLuaVM::debug(const char *s)
{
    if (s && m_callback) {
        m_callback(s);
    }
}

void CLuaVM::debugv(const char *fmt, ...)
{
    char *s = nullptr;
    va_list args;
    va_start(args, fmt);
    if (vasprintf(&s, fmt, args) == -1) {
        qDebug("debugv() had an allocation error with vasprintf().");
    }
    if (s) {
        debug(static_cast<const char*>(s));
        free(s);
    }
}

/////////////////////////////////////////////////////////////////////////////
// lua interface

int CLuaVM::debug(lua_State *L)
{
    int argc = lua_gettop(L);
    for (int n=1; n <= argc; ++n) {
        const char *s = lua_tostring(L, n);
        if (s && m_callback) {
            m_callback(s);
        }
    }
    return 0;
}

void CLuaVM::clearLog()
{
    // TODO: implement this
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

void CLuaVM::setCallback(std::function<void(const char *)> callback)
{
    m_callback = callback;
}
