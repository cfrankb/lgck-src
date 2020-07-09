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

#ifndef _LUAVM_H__
#define _LUAVM_H__

#include <stdio.h>
#include <functional>

extern "C" {
#include <lua5.2/lauxlib.h>
#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
}

class CLuaVM
{
public:

    CLuaVM();
    ~CLuaVM();
    lua_State * getState();
    int exec(const char *luaCode);

    void debug(const char *s);
    void clearLog();
    void registerFn(const char * fnName, lua_CFunction fn);
    void registerInt(const char *name, int value);
    void registerString(const char *name, const char *value);
    void registerBool(const char* name, bool value);
    void reportErrors( int status, const char *fnName = nullptr);

    // lua interface
    static void setCallback(std::function<void(const char *)> callback);
    static int debug(lua_State *L);

protected:
    static std::function<void(const char *)> m_callback;
    lua_State *m_luaState;
};

#endif
