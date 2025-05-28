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

#include <cstdio>
#include <functional>

#ifdef EMS
extern "C" {
#include "extern/lua5.4/lauxlib.h"
#include "extern/lua5.4/lua.h"
#include "extern/lua5.4/lualib.h"
}
#else
extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}
#endif

#include <unordered_map>

class CLuaVM
{
public:

    CLuaVM();
    ~CLuaVM();
    lua_State * getState();
    int exec(const char *luaCode);

    void clearLog();
    void registerFn(const char * fnName, lua_CFunction fn);
    void registerInt(const char *name, int value);
    void registerString(const char *name, const char *value);
    void registerBool(const char* name, bool value);
    void reportErrors( int status, const char *fnName = nullptr);

    // lua interface
    enum Callback {
        Debug,
        Error
    };

    typedef std::function<void(const char *)> CallbackFct;
    static void setCallback(int callbackID, CallbackFct callback);
    static int debug(lua_State *L);
    static void debug(const char *s);
    static void error(const char *s);
    static void debugv(const char *fmt, ...);

protected:
    static std::unordered_map<int, CallbackFct> m_callback;
    lua_State *m_luaState;
};

#endif
