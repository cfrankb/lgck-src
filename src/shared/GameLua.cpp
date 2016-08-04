/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2012  Francois Blanchette

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include "Game.h"
#include "FileWrap.h"
#include "Folders.h"
#include "Inventory.h"
#include "Map.h"
#include "Scene.h"
#include "Snd.h"
#include "Frame.h"
#include "StringTable.h"
#include "Level.h"
#include "Layer.h"
#include "GameEvents.h"
#include "GameLua.h"
#include "Display.h"
#include "Snapshot.h"
#include "interfaces/IMusic.h"
#include "interfaces/IDisplayManager.h"
#include "interfaces/IGraphics.h"

#ifdef USE_QFILE
    #include <QMessageBox>
#endif

/////////////////////////////////////////////////////////////////////////////
// static variables

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

CGame * CGame::m_game;
CLuaVM CGame::m_lua;

/////////////////////////////////////////////////////////////////////////////
// lua interface

void CGame::initLua()
{
    qDebug("initLua");
    // registers functions
    for (int i = 0; exports[i].fnName; ++i) {
        m_lua.registerFn(exports[i].fnName, exports[i].fnAddr);
    }
    std::string s;
    generateRuntimeLua(s);
    m_lua.exec(s.c_str());
}

int playSound(lua_State *L)
{
    const char *fn = "playSound";
    int argc = lua_gettop(L);
    if ( argc < 1)  {
        CGame::error(fn, 1);
    } else {
        for ( int n=1; n <= argc; ++n ) {
            if (lua_isnumber(L, n)) {
                CGame::getGame().playSound(lua_tointeger(L, n));
            } else {
                CGame::getGame().playSound(lua_tostring(L, n));
            }
        }
    }
    return 0;
}

int ss_notifyClosure(lua_State *L)
{
    const char *fn = "ss_notifyClosure";
    int argc = lua_gettop(L);
    if ( argc != 0)  {
        CGame::error(fn, 0);
    }
    CGame::getGame().callGameEvent(CGameEvents::EG_NOTIFYCLOSURE);
    CGame::getGame().callLvEvent(CLevel::EL_NOTIFYCLOSURE);
    CGame::getGame().scene().notifyClosure();
    return 0;
}

int countGoals(lua_State *L)
{
    const char *fn = "countGoals";
    int argc = lua_gettop(L);
    if ( argc != 0)  {
        CGame::error(fn, 0);
    }
    lua_pushnumber(L, CGame::getGame().scene().countGoals());
    return 1;
}

int cancelEvent(lua_State *L)
{
    Q_UNUSED( L );
    return 0;
}

int callTrigger(lua_State *L)
{
    const char *fn = "callTrigger";
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error(fn, 1);
    } else {
        game.callTrigger( (int) lua_tonumber(L, 1) );
    }
    return 0;
}

int sprite_setTriggerKey(lua_State *L)
//# setTriggerKey
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 2)  {
        CGame::error("sprite_setTriggerKey", 2);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        int key = CGame::TRIGGER_KEYS & (int) (int) lua_tonumber(L, 2) ;
        entry.m_nTriggerKey = key | (entry.m_nTriggerKey & ( 0xff ^ CGame::TRIGGER_KEYS));
    }
    return 0;
}

int sprite_freeze(lua_State *L)
//# freezeSprite
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("sprite_freeze", 1);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene[ (int) lua_tonumber(L, 1) ];
        entry.m_nTriggerKey |= CGame::TRIGGER_FROZEN;
    }
    return 0;
}

int sprite_hide(lua_State *L)
//# hideSprite
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("sprite_hide", 1);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.m_nTriggerKey |= CGame::TRIGGER_HIDDEN;
    }
    return 0;
}

int sprite_show(lua_State *L)
//# showSprite
{
    const char *fn = "sprite_show";
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error(fn, 1);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.m_nTriggerKey &= (-1 ^ CGame::TRIGGER_HIDDEN);
    }
    return 0;
}

int sprite_unFreeze(lua_State *L)
//# unFreezeSprite
{
    const char *fn = "sprite_unFreeze";
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error(fn, 1);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.m_nTriggerKey &= (-1 ^ CGame::TRIGGER_FROZEN);
    }
    return 0;
}

int sprite_activate(lua_State *L)
//# activateSprite
{
    const char *fn = "sprite_activate";
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error(fn, 1);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.activate();
    }
    return 0;
}

int sprite_markAsGoal(lua_State *L)
//# markSpriteAsGoal
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("sprite_markAsGoal", 1);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.m_nTriggerKey |= CGame::TRIGGER_GOAL;
    }
    return 0;
}

int getTickScale(lua_State *L)
{
    CGame & game = CGame::getGame();
    lua_pushnumber(L, game.var("TICK_SCALE"));
    return 1; // number of return values
}

int setTickScale(lua_State *L)
{
    const char *fnName = "setTickScale";
    CGame & game = CGame::getGame();

    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        game.var("TICK_SCALE") = (int) lua_tonumber(L, 1);
    }

    return 0;
}

int setSpeed(lua_State *L)
{
    const char *fnName = "setSpeed";
    CGame & game = CGame::getGame();

    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        int rate = (int) lua_tonumber(L, 1);
        if (rate) {
            game.var("TICK_SCALE") = 1000 / rate;
        }else {
            CGame::debug("-- Game speed cannot be zero");
        }
    }

    return 0;
}

int sprite_getObjType(lua_State *L)
//# getObjType
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    for ( int n=1; n <= argc; ++n ) {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, n) ];
        lua_pushnumber(L, entry.m_nProto);
    }

    return argc;
}

int sprite_testFlags(lua_State *L)
//# testFlags
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if (argc < 2) {
        CGame::error("testFlags", 2);
        lua_pushboolean(L, 0);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        int mask = (int) lua_tonumber(L, 2);
        lua_pushboolean(L, entry.getFlags() & mask);
    }

    return 1;
}

int sprite_new(lua_State *L)
//# addSpriteC
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if (argc < 4) {
        CGame::error("sprite_new", 4);
        lua_pushnumber(L, 0);
    } else {
        CActor entry;
        entry.m_nX = (int) lua_tonumber(L, 1) ;
        entry.m_nY = (int) lua_tonumber(L, 2);
        entry.m_nAim = (int) lua_tonumber(L, 3);
        entry.m_nProto = (int) lua_tonumber(L, 4);
        if (argc >= 6) {
            entry.m_nFrameSet = (int) lua_tonumber(L, 5);
            entry.m_nFrameNo = (int) lua_tonumber(L, 6);
        } else {
            CProto & proto = game.m_arrProto[entry.m_nProto];
            entry.m_nFrameSet = proto.m_nFrameSet;
            entry.m_nFrameNo = proto.m_nFrameNo;
        }
        lua_pushnumber(L, game.scene().add( entry ));
    }

    return 1;
}

int sprite_getVars(lua_State *L)
//# getSpriteVars
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if (argc < 1) {
        CGame::error("sprite_getVars", 1);
        lua_pushnumber(L, 0);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        lua_pushnumber(L, entry.m_nX);
        lua_pushnumber(L, entry.m_nY);
        lua_pushnumber(L, entry.m_nAim);

        lua_pushnumber(L, entry.m_nProto);
        lua_pushnumber(L, entry.m_nFrameSet);
        lua_pushnumber(L, entry.m_nFrameNo);

        lua_pushnumber(L, entry.m_nTriggerKey);
        lua_pushnumber(L, entry.m_nStateFlag);
        lua_pushnumber(L, entry.m_nActionMask);
    }
    return 9;
}

int testJoyState(lua_State *L)
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if (argc < 1) {
        CGame::debug("-- warning: getJoyState(...) requires 1 args.");
        lua_pushboolean(L, 0);
    } else {
        int mask = (int) lua_tonumber(L, 1);
        lua_pushboolean(L, game.getJoyState() & mask);
    }
    return 1;
}

int sprite_getName(lua_State *L)
//# getObjName
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    for ( int n=1; n <= argc; ++n ) {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, n) ];
        lua_pushstring(L, game.m_arrProto[entry.m_nProto].getName());
    }
    return argc;
}

int sprite_getClass(lua_State *L)
//# getObjClass
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    for ( int n=1; n <= argc; ++n ) {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, n) ];
        lua_pushnumber(L, game.m_arrProto[entry.m_nProto].m_nClass);
    }
    return argc;
}

int scene_getSize(lua_State *L)
//# getObjCount
//# getSpriteCount
{
    CGame & game = CGame::getGame();
    lua_pushnumber(L, game.scene().getSize());
    return 1;
}


int killPlayer(lua_State *)
{
    CGame & game = CGame::getGame();
    CScene & scene = game.scene();
    CActor & player = scene [ game.svar("playerEntry") ] ;
    game.killPlayer( player );
    return 0;
}

int sprite_move(lua_State *L)
//# moveSprite
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if (argc < 2) {
        CGame::error("sprite_move", 2);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        if (argc == 2) {
            entry.move( (int) lua_tonumber(L, 2) );
        } else {
            entry.moveBy( (int) lua_tonumber(L, 2), (int) lua_tonumber(L, 3));
        }
    }
    return 0;
}

int sprite_tryAnimation(lua_State *L)
//# tryAnimation
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);

    if (argc != 2) {
        CGame::error("sprite_tryAnimation", 2);
        lua_pushboolean(L, false);
    } else {
        int objId = (int) lua_tonumber(L, 1);
        int animSeq = (int) lua_tonumber(L, 2);
        if (animSeq >= 0 && animSeq < CObject::getAnimationCount()) {
            CScene & scene = game.scene();
            bool result = scene[objId].tryAnimation(animSeq);
            lua_pushboolean(L,  result);
        } else  {
            CGame::debug("-- sprite_tryAnimation() was passed an invalid value for animSeq");
            lua_pushboolean(L, false);
        }
    }
    return 1;
}

int sprite_moveTo(lua_State *L)
//# moveSpriteTo
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);

    if (argc != 3 ) {
        CGame::error("sprite_moveTo", 3);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.moveTo( (int) lua_tonumber(L, 2), (int) lua_tonumber(L, 3));
    }

    return 0;
}

int sprite_moveBy(lua_State *L)
//# moveSpriteBy
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);

    if (argc != 3 ) {
        CGame::error("sprite_moveBy", 3);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.moveBy( (int) lua_tonumber(L, 2), (int) lua_tonumber(L, 3));
    }
    return 0;
}

int sprite_canMove(lua_State *L)
//# canMove
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 2)  {
        CGame::error("sprite_canMove", 2);
        lua_pushboolean(L, 0);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        lua_pushboolean(L, entry.canMove((int) lua_tonumber(L, 2) , true));
    }

    return 1;
}

int sprite_canFall(lua_State *L)
//# canFall
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("sprite_canFall", 1);
        lua_pushboolean(L, 0);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        lua_pushboolean(L, entry.canFall() );
    }

    return 1;
}

int testAim(lua_State *L)
{
    Q_UNUSED(L);
    return 0;
}

int sprite_map(lua_State *L)
//# mapSprite
{
    const char *fn = "sprite_map";
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc < 1)  {
        CGame::error(fn, 1);
    } else {
        CScene & scene = game.scene();
        for ( int n = 1; n <= argc; ++ n ) {
            scene[ (int) lua_tonumber(L, n) ].map();
        }
    }
    return 0;
}

int sprite_unMap(lua_State *L)
//#unMapSprite
{
    const char *fn = "sprite_unMap";
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc < 1)  {
        CGame::error(fn, 1);
    } else {
        CScene & scene = game.scene();
        for ( int n=1; n <= argc; ++n ) {
            scene [ (int) lua_tonumber(L, n) ].unMap();
        }
    }
    return argc;
}

int XOR(lua_State *L)
{
    int argc = lua_gettop(L);
    if ( argc != 2)  {
        CGame::error("XOR", 2);
        lua_pushnumber(L, 0);
    } else {
        int a = (int) lua_tonumber(L, 1);
        int b = (int) lua_tonumber(L, 2);
        lua_pushnumber(L, a ^ b);
    }

    return 1;
}

int OR(lua_State *L)
{
    int argc = lua_gettop(L);
    if ( argc != 2)  {
        CGame::error("OR", 2);
        lua_pushnumber(L, 0);
    } else {
        int a = (int) lua_tonumber(L, 1);
        int b = (int) lua_tonumber(L, 2);
        lua_pushnumber(L, a | b);
    }
    return 1;
}

int AND(lua_State *L)
{
    int argc = lua_gettop(L);
    if ( argc != 2)  {
        CGame::error("AND", 2);
        lua_pushnumber(L, 0);
    } else {
        int a = (int) lua_tonumber(L, 1);
        int b = (int) lua_tonumber(L, 2);
        lua_pushnumber(L, a & b);
    }

    return 1;
}

int sprite_setAim(lua_State *L)
//# setAim
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 2)  {
        CGame::error("sprite_setAim", 2);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.m_nAim = (int) lua_tonumber(L, 2);
    }

    return 0;
}

int sprite_isPlayerThere(lua_State *L)
//#isPlayerThere
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 2)  {
        CGame::error("sprite_isPlayerThere", 2);
        lua_pushboolean(L, false);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        lua_pushboolean(L, entry.isPlayerThere( (int) lua_tonumber(L, 2)));
    }

    return 1;
}

int sprite_attackPlayer(lua_State *L)
//# attackPlayer
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("sprite_attackPlayer", 1);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.attackPlayer();
    }
    return 0;
}

int sprite_isVisible(lua_State *L)
//# isVisible
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("sprite_isVisible", 1);
        lua_pushboolean(L, false);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        lua_pushboolean(L, entry.isVisible());
    }

    return 1;
}

int sprite_getTriggerKey(lua_State *L)
//# getTriggerKey
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("sprite_getTriggerKey", 1);
        lua_pushnumber(L, 0);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        lua_pushnumber(L, (int)entry.m_nTriggerKey & CGame::TRIGGER_KEYS);
    }

    return 1;
}

int addToScore(lua_State *L)
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("addToScore", 1);
        lua_pushnumber(L, game.getScore());
    } else {
        game.addToScore( (int) lua_tonumber(L, 1) );
        lua_pushnumber(L, game.getScore());
    }

    return 1;
}

int getScore(lua_State *L)
{
    CGame & game = CGame::getGame();
    lua_pushnumber(L, game.getScore());
    return 1;
}

int getHP(lua_State *L)
{
    CGame & game = CGame::getGame();
    CScene & scene = game.scene();
    CActor & player = scene [ game.svar("playerEntry") ];
    lua_pushnumber(L, player.getHP());
    return 1;
}

int setHP(lua_State *L)
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("setHP", 1);
    } else {
        CScene & scene = game.scene();
        CActor & player = scene [ game.svar("playerEntry") ];
        player.setHP((int) lua_tonumber(L, 1));
    }

    return 0;
}

int addToHP(lua_State *L)
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("addTotHP", 1);
    } else {
        game.addToHP( (int) lua_tonumber(L, 1) );
    }

    CScene & scene = game.scene();
    CActor & player = scene [ game.svar("playerEntry") ];
    lua_pushnumber(L, player.getHP());
    return 1;
}

int display_new(lua_State *L)
//# addDisplayC
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if (argc != 4) {
        CGame::error("display_new", 4);
        lua_pushnumber(L, -1);
    } else {
        int id = game.displays()->getSize();
        game.displays()->add(
                lua_tostring(L, 1),
                (int) lua_tonumber(L, 2),
                (int) lua_tonumber(L, 3),
                (int) lua_tonumber(L, 4)
        );
        lua_pushnumber(L, id);
    }

    return 1;
}

int display_move(lua_State *L)
//#displaySetXY
{
    const char *fnName = "display_move";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
    } else {       
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CDisplay & display = CGame::getGame().displays()->getAt(id);
            display.setXY((int) lua_tonumber(L, 2), (int) lua_tonumber(L, 3));
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }

    return 0;
}

int display_setType(lua_State *L)
//# displaySetType
{
    const char *fnName = "display_setType";

    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
    } else {
        int id = (int) lua_tonumber(L, 1);
        IDisplayManager * manager = CGame::getGame().displays();
        if (manager->isValidIndex( id )) {
            CDisplay & display = manager->getAt(id);
            display.setType((int) lua_tonumber(L, 2));
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }

    return 0;
}

int display_setColor(lua_State *L)
//# displaySetColor
{
    const char *fnName = "display_setColor";
    int argc = lua_gettop(L);
    if (argc != 5) {
        CGame::error(fnName, 5);
    } else {
        int id = (int) lua_tonumber(L, 1);
        IDisplayManager * manager = CGame::getGame().displays();
        if (manager->isValidIndex( id )) {
            CDisplay & display = manager->getAt(id);
            display.setColor(
                    (int) lua_tonumber(L, 2),
                    (int) lua_tonumber(L, 3),
                    (int) lua_tonumber(L, 4),
                    (int) lua_tonumber(L, 5)
            );
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }

    return 0;
}

int display_setExpireTime(lua_State *L)
//# displaySetExpireTime
{
    const char *fnName = "display_setExpireTime";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
    } else {
        int id = (int) lua_tonumber(L, 1);
        IDisplayManager * manager = CGame::getGame().displays();
        if (manager->isValidIndex( id )) {
            CDisplay & display = manager->getAt(id);
            display.setExpireTime((int) lua_tonumber(L, 2) );
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }

    return 0;
}

int display_setFontSize(lua_State *L)
//# displaySetFontSize
{
    const char *fnName = "display_setFontSize";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
    } else {
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CDisplay & display = CGame::getGame().displays()->getAt(id);
            display.setFontSize((int) lua_tonumber(L, 2) );

        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }
    return 0;
}

int display_setAlpha(lua_State *L)
//# displaySetAlpha
{
    const char *fnName = "display_setAlpha";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
    } else {
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CDisplay & display = CGame::getGame().displays()->getAt(id);
            display.setAlpha((int) lua_tonumber(L, 2) );
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }

    return 0;
}

int display_setVisible(lua_State *L)
//# displaySetVisible
{
    const char *fnName = "display_setVisible";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
    } else {
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CDisplay & display = CGame::getGame().displays()->getAt(id);
            display.setVisible(lua_toboolean(L, 2) );
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }

    return 0;
}

int display_setText(lua_State *L)
//# displaySetText
{
    const char *fnName = "display_setText";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
    } else {
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CDisplay & display = CGame::getGame().displays()->getAt(id);
            display.setText(lua_tostring(L, 2) );
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }

    return 0;
}

int display_setImage(lua_State *L)
//# displaySetImage
{
    const char *fnName = "display_setImage";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
    } else {
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CDisplay & display = CGame::getGame().displays()->getAt(id);
            display.setImage(lua_tonumber(L, 2), lua_tonumber(L, 3) );
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }

    return 0;
}

int display_setShadowOffset(lua_State *L)
//#displaySetShadowOffset
{
    const char *fnName = "display_setShadowOffset";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
    } else {
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CDisplay & display = CGame::getGame().displays()->getAt(id);
            display.setShadowOffset(lua_tonumber(L, 2), lua_tonumber(L, 3) );
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }
    return 0;
}

int display_setShadowColor(lua_State *L)
//# displaySetShadowColor
{
    const char *fnName = "display_setShadowColor";
    // r g b a
    int argc = lua_gettop(L);
    if (argc != 5) {
        CGame::error(fnName, 5);
    } else {
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CDisplay & display = CGame::getGame().displays()->getAt(id);
            display.setShadowColor(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5) );
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }
    return 0;
}

int display_enableShadows(lua_State *L)
//# displayEnableShadows
{
    const char *fnName = "display_enableShadows";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
    } else {
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CDisplay & display = CGame::getGame().displays()->getAt(id);
            display.enableShadow( lua_toboolean(L, 2) );
        } else {
            char tmp[1024];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s``", id, fnName);
            CGame::debug(tmp);
        }
    }

    return 0;
}

int findDisplay(lua_State *L)
{
    const char *fnName = "findDisplay";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
    } else {
        const char* name = lua_tostring(L, 1);
        lua_pushnumber(L, CGame::getGame().displays()->findDisplay(name));
    }
    return 1;
}

int setEndLevel(lua_State *L)
{
    const char *fnName = "setEndLevel";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        lua_pushnumber(L, -1);
    } else {
        CGame::getGame().setEndLevel( true );
    }
    return 1;
}

int getTime(lua_State *L)
{
    unsigned long long tm;
    microtime(&tm);
    lua_pushnumber(L, tm - CGame::getGame().m_startTime);
    return 1;
}

int getTicks(lua_State *L)
{
    lua_pushnumber(L, CGame::getGame().getTickCount());
    return 1;
}

int getWrapFlag(lua_State *L)
{
    lua_pushnumber(L, CGame::getGame().var("wrapFlags"));
    return 1;
}

int setWrapFlag(lua_State *L)
{
    const char *fnName = "setWrapFlag";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        CGame::getGame().var("wrapFlags") = (int) lua_tonumber(L, 1);
    }
    return 0;
}

int layer_new(lua_State *L)
//# addLayerC
{
    const char *fnName = "layer_new";
    int argc = lua_gettop(L);
    if (argc < 2 || argc > 4) {
        CGame::error(fnName, 2);
    } else {
        const char * name =  lua_tostring(L, 1);
        int type = lua_tonumber(L, 2);
        int h = 0;
        int v = 0;

        if (argc > 2) {
            h = lua_tonumber(L, 3);
            v = lua_tonumber(L, 4);
        }

        CLevel * layers = CGame::getGame().getLayers();
        CLayer * layer = new CLayer(name, type, h, v );
        int id = layers->addLayer( layer );
        lua_pushnumber(L, id);
        return 1;
    }

    lua_pushnumber(L, -1);
    return 1;
}


int findLayer(lua_State *L)
//#getLayerC
{
    const char *fnName = "findLayer";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        const char * name =  lua_tostring(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        for (int i=0; i < layers->getSize(); ++i) {
            if (!strcmp((*layers)[i].getName(), name)) {
                lua_pushnumber(L, i);
                return 1;
            }
        }
        char tmp[2048];
        sprintf(tmp, "%s(...) - can't find fn `%s`", fnName, name);
        CGame::debug(tmp);
    }
    lua_pushnumber(L, -1);
    return 1;
}

int element_new(lua_State *L)
//#addElement
{
    const char *fnName = "element_new";
    int argc = lua_gettop(L);
    if (argc < 5 || argc > 6) {
        lua_pushnumber(L, -1);
        CGame::error(fnName, 5);
    } else {
        int id =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((id < 0) || (id >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, id);
            CGame::debug(tmp);
            lua_pushnumber(L, -1);
        } else {
            int imageSet =  lua_tonumber(L, 2);
            int imageNo =  lua_tonumber(L, 3);
            int x = lua_tonumber(L, 4);
            int y = lua_tonumber(L, 5);
            bool show = false;
            if (argc == 6) {
                show = lua_toboolean(L, 6);
            }
            lua_pushnumber(L, layers[id].getSize());
            CLevelEntry entry(imageSet, imageNo, x, y, show);
            (*layers)[id].add(entry);
        }
    }
    return 1;
}

int layer_clear(lua_State *L)
{
    const char *fnName = "layer_clear";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushboolean(L, false);
    } else {
        int id =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((id < 0) || (id >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, id);
            CGame::debug(tmp);
            lua_pushboolean(L, false);
        } else {
            (*layers)[id].forget();
            lua_pushboolean(L, true);
        }
    }
    return 1;
}

int layer_getSize(lua_State *L)
{
    const char *fnName = "layer_getSize";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
    } else {
        int id =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((id < 0) || (id >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, id);
            CGame::debug(tmp);
            lua_pushnumber(L, -1);
        } else {
            lua_pushnumber(L, (*layers)[id].getSize());
        }
    }
    return 1;
}


int layer_getElement(lua_State *L)
{
    const char *fnName = "layer_getElement";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        int layerId =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((layerId < 0) || (layerId >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, layerId);
            CGame::debug(tmp);
          //  lua_pushnumber(L, -1);
            return 0;
        } else {
            CLayer & layer = (*layers)[layerId];
            int id =  lua_tonumber(L, 2);
            if (id < 0 || id >= layer.getSize()) {
                char tmp[2048];
                sprintf(tmp, "%s(...) - elementId `%d` is out of bound", fnName, id);
                CGame::debug(tmp);
                //  lua_pushnumber(L, -1);
                return 0;
            } else {
                CLevelEntry & e = layer[id];
                lua_pushnumber(L, e.m_nFrameSet);
                lua_pushnumber(L, e.m_nFrameNo);
                lua_pushnumber(L, e.m_nX);
                lua_pushnumber(L, e.m_nY);
                lua_pushnumber(L, !(e.m_nTriggerKey & 0x80) );
            }
        }
    }
    return 5;
}

int element_setImage(lua_State *L)
{
    const char *fnName = "element_setImage";
    int argc = lua_gettop(L);
    if (argc != 4) {
        CGame::error(fnName, 4);
        lua_pushboolean(L, false);
        return 1;
    } else {
        int layerId =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((layerId < 0) || (layerId >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, layerId);
            CGame::debug(tmp);
            lua_pushboolean(L, false);
            return 1;
        } else {
            CLayer & layer = (*layers)[layerId];
            int id =  lua_tonumber(L, 2);
            if (id < 0 || id >= layer.getSize()) {
                char tmp[2048];
                sprintf(tmp, "%s(...) - elementId `%d` is out of bound", fnName, id);
                CGame::debug(tmp);
                lua_pushboolean(L, false);
                return 1;
            } else {
                CLevelEntry & e = layer[id];
                e.m_nFrameSet = (int) lua_tonumber(L, 3);
                e.m_nFrameNo = (int) lua_tonumber(L, 4);
            }
        }
    }
    lua_pushboolean(L, true);
    return 1;
}

int element_setVisible(lua_State *L)
{
    const char *fnName = "element_setVisible";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
        lua_pushboolean(L, false);
        return 1;
    } else {
        int layerId =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((layerId < 0) || (layerId >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, layerId);
            CGame::debug(tmp);
            lua_pushboolean(L, false);
            return 1;
        } else {
            CLayer & layer = (*layers)[layerId];
            int id =  lua_tonumber(L, 2);
            if (id < 0 || id >= layer.getSize()) {
                char tmp[2048];
                sprintf(tmp, "%s(...) - elementId `%d` is out of bound", fnName, id);
                CGame::debug(tmp);
                lua_pushboolean(L, false);
                return 1;
            } else {
                CLevelEntry & e = layer[id];
                bool show = (bool) lua_toboolean(L, 3);
                if (show) {
                    e.m_nTriggerKey = 0;
                } else {
                    e.m_nTriggerKey = 0x80;
                }
            }
        }
    }

    lua_pushboolean(L, true);
    return 1;
}

int element_moveTo(lua_State *L)
{
    const char *fnName = "element_moveTo";
    int argc = lua_gettop(L);
    if (argc != 4) {
        CGame::error(fnName, 4);
        lua_pushboolean(L, false);
        return 1;
    } else {
        int layerId =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((layerId < 0) || (layerId >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, layerId);
            CGame::debug(tmp);
            lua_pushboolean(L, false);
            return 1;
        } else {
            CLayer & layer = (*layers)[layerId];
            int id =  lua_tonumber(L, 2);
            if (id < 0 || id >= layer.getSize()) {
                char tmp[2048];
                sprintf(tmp, "%s(...) - elementId `%d` is out of bound", fnName, id);
                CGame::debug(tmp);
                lua_pushboolean(L, false);
                return 1;
            } else {
                CLevelEntry & e = layer[id];
                e.m_nX = (int) lua_tonumber(L, 3);
                e.m_nY = (int) lua_tonumber(L, 4);
            }
        }
    }

    lua_pushboolean(L, true);
    return 1;
}

int element_move(lua_State *L)
{
    const char *fnName = "element_move";
    int argc = lua_gettop(L);
    if (argc != 4) {
        CGame::error(fnName, 4);
        lua_pushboolean(L, false);
        return 1;
    } else {
        int layerId =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((layerId < 0) || (layerId >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, layerId);
            CGame::debug(tmp);
            lua_pushboolean(L, false);
            return 1;
        } else {
            CLayer & layer = (*layers)[layerId];
            int id =  lua_tonumber(L, 2);
            if (id < 0 || id >= layer.getSize()) {
                char tmp[2048];
                sprintf(tmp, "%s(...) - elementId `%d` is out of bound", fnName, id);
                CGame::debug(tmp);
                lua_pushboolean(L, false);
                return 1;
            } else {
                CLevelEntry & e = layer[id];
                int aim = (int) lua_tonumber(L, 3);
                switch (aim) {
                case CGame::UP:
                    e.m_nY -= 8;
                    break;

                case CGame::DOWN:
                    e.m_nY += 8;
                    break;

                case CGame::LEFT:
                    e.m_nX -= 8;
                    break;

                case CGame::RIGHT:
                    e.m_nX += 8;
                    break;

                default:
                    char tmp[2048];
                    sprintf(tmp, "%s(...) - aim value `%d` is invalid", fnName, aim);
                    CGame::debug(tmp);
                    lua_pushboolean(L, false);
                    return 1;
                    break;

                }
            }
        }
    }

    lua_pushboolean(L, true);
    return 1;
}

int element_moveBy(lua_State *L)
{
    const char *fnName = "element_moveBy";
    int argc = lua_gettop(L);
    if (argc != 4) {
        CGame::error(fnName, 4);
        lua_pushboolean(L, false);
        return 1;
    } else {
        int layerId =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((layerId < 0) || (layerId >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, layerId);
            CGame::debug(tmp);
            lua_pushboolean(L, false);
            return 1;
        } else {
            CLayer & layer = (*layers)[layerId];
            int id =  lua_tonumber(L, 2);
            if (id < 0 || id >= layer.getSize()) {
                char tmp[2048];
                sprintf(tmp, "%s(...) - elementId `%d` is out of bound", fnName, id);
                CGame::debug(tmp);
                lua_pushboolean(L, false);
                return 1;
            } else {
                CLevelEntry & e = layer[id];
                e.m_nX += (int) lua_tonumber(L, 3);
                e.m_nY += (int) lua_tonumber(L, 4);
            }
        }
    }

    lua_pushboolean(L, true);
    return 1;
}

int setLevelGoal(lua_State *L)
{
    const char *fnName = "setLevelGoal";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        int levelGoal =  lua_tonumber(L, 1);
        CGame::getGame().setLevelGoal(levelGoal);
    }
    return 0;
}

int getLevelGoal(lua_State *L)
{
    lua_pushnumber(L, CGame::getGame().getLevelGoal());
    return 1;
}

int getGoalLeft(lua_State *L)
{
    lua_pushnumber(L, CGame::getGame().counter("goalLeft"));
    return 1;
}

int hasGravity(lua_State *L)
{
    lua_pushboolean(L, CGame::getGame().gravity());
    return 1;
}

int rand(lua_State *L)
{
    lua_pushnumber(L, ::rand());
    return 1;
}

int getBkColor(lua_State *L)
//#getBkColorC
{
    CGame & game = CGame::getGame();
    UINT32 bkColor = game.getBkColor();
    int red =  bkColor & 0xff;
    int green = (bkColor & 0xff00) >> 8;
    int blue = (bkColor & 0xff0000) >> 16;
    lua_pushnumber(L, red);
    lua_pushnumber(L, green);
    lua_pushnumber(L, blue);
    return 3;
}

int setBkColor(lua_State *L)
//#setBkColorC
{
    const char *fnName = "setBkColor";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
    } else {
        CGame & game = CGame::getGame();
        int red =  ((int)lua_tonumber(L, 1)) & 0xff;
        int green =  ((int)lua_tonumber(L, 2)) & 0xff;
        int blue =  ((int)lua_tonumber(L, 3)) & 0xff;
        UINT32 bkColor = red + (green << 8) + (blue << 16) + 0xff000000;
        game.setBkColor(bkColor);
    }
    return 0;
}

int sprite_copy(lua_State *L)
//#copySpriteC
{
    const char *fnName = "sprite_copy";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
        return 1;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = game.scene();
        CActor & src = scene[objId];
        // TODO: garenteed to crash
        int newId = scene.add(src);
        lua_pushnumber(L, newId);
        return 1;
    }
}

int findSprite(lua_State *L)
{
    const char *fnName = "findSprite";
    int argc = lua_gettop(L);
    if (argc < 1 && argc > 2) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
        return 1;
    } else {
        CGame & game = CGame::getGame();
        int objClass = (int) lua_tonumber(L, 1);
        int origin = 0;

        if (argc == 2) {
            origin = (int) lua_tonumber(L, 2);
        }

        if (origin < 0) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - origin `%d` is out of bound", fnName, origin);
            CGame::debug(tmp);
        }

        CScene & scene = game.scene();
        for (int i = origin; i < scene.getSize(); ++i) {
            CActor & src = scene[i];
            CProto & proto = game.m_arrProto[src.m_nProto];
            if (proto.m_nClass == objClass) {
                lua_pushnumber(L, i);
                return 1;
            }
        }

        lua_pushnumber(L, -1);
        return 1;
    }
}


int sprite_setState(lua_State *L)
//#setState
{
    const char *fnName = "sprite_setState";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        int stateFlag = (int) lua_tonumber(L, 2);
        bool flip = (int) lua_toboolean(L, 3);
        CScene & scene = game.scene();
        CActor & src = scene[objId];
        int result = src.setState(stateFlag, flip);
        lua_pushnumber(L, result);
        return 1;
    }
}

int getImage(lua_State *L)
{
    const char *fnName = "getImage";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
        return 1;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = game.scene();
        CActor & src = scene[objId];
        lua_pushnumber(L, src.m_nFrameSet);
        lua_pushnumber(L, src.m_nFrameNo);
        return 2;
    }
}

int sprite_setImage(lua_State *L)
//#setImage
{
    const char *fnName = "sprite_setImage";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
        lua_pushnumber(L, -1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = game.scene();
        CActor & src = scene[objId];
        src.m_nFrameSet = (int)lua_tonumber(L, 2);
        src.m_nFrameNo = (int)lua_tonumber(L, 3);
        return 0;
    }
}

int getLookUp(lua_State *L)
{
    lua_pushnumber(L, CGame::getGame().getLookUp());
    return 1;
}

int setLookUp(lua_State *L)
{
    const char *fnName = "setLookUp";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 3);
    } else {
        CGame & game = CGame::getGame();
        bool lookup = lua_toboolean(L, 1);
        game.setLookUp(lookup);
    }
    return 0;
}

int clearDisplay(lua_State *)
{
    CGame::getGame().clearDisplay();
    return 0;
}

int display_remove(lua_State *L)
//#removeDisplayById
{
    const char *fnName = "display_remove";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        int id = (int) lua_tonumber(L, 1);
        if (CGame::getGame().displays()->isValidIndex( id )) {
            CGame::getGame().displays()->removeAt( id );
        } else {
            char tmp[2048];
            sprintf(tmp, "-- displayId ``%d`` not valid for ``%s", id, fnName);
            CGame::debug(tmp);
        }
    }
    return 0;
}

int pause(lua_State *)
{
    CGame::getGame().flipPause();
    return 0;
} 

int addToInventory(lua_State * L)
{
    const char *fnName = "addToInventory";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        int protoId = (int) lua_tonumber(L, 1);
        char tmp[2048];
        sprintf(tmp,"-- adding type ``%d to inventory" , protoId);
        CGame::debug(tmp);
        CInventory *inventory = CGame::getGame().getInventory();
        if (inventory){
            inventory->addItem(protoId);
        } else {
            CGame::debug("can't find inventory @player");
        }
    }
    return 0;
}

int sprite_addItem(lua_State * L)
{
    const char *fnName = "sprite_addItem";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = game.scene();
        CActor & src = scene[objId];
        int protoId = (int) lua_tonumber(L, 2);
        int count = (int) lua_tonumber(L, 3);
        char tmp[2048];
        sprintf(tmp,"-- adding type ``%d to inventory" , protoId);
        CGame::debug(tmp);
        CInventory *inventory = src.getInventory();
        if (inventory){
            inventory->addItem(protoId,count);
        } else {
            sprintf(tmp,"can't find inventory for sprite %d", objId);
            CGame::debug(tmp);
        }
    }
    return 0;
}

int hasItem( lua_State * L)
{
    const char *fnName = "hasItem";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushboolean(L, false);
    } else {
        int protoId = (int) lua_tonumber(L, 1);
        CInventory *inventory = CGame::getGame().getInventory();
        lua_pushboolean(L, inventory ? inventory->hasItem(protoId): 0);
    }
    return 1;
}

int sprite_hasItem( lua_State * L)
{
    const char *fnName = "sprite_hasItem";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        lua_pushboolean(L, false);
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = CGame::getGame().scene();
        CActor & src = scene[objId];
        int protoId = (int) lua_tonumber(L, 2);
        CInventory *inventory = src.getInventory();
        lua_pushboolean(L, inventory ? inventory->hasItem(protoId): 0);
    }
    return 1;
}

int resetInventory(lua_State * L)
{
    const char *fnName = "resetInventory";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        lua_pushboolean(L, false);
    } else {
        CInventory *inventory = CGame::getGame().getInventory();
        if (inventory) {
            inventory->reset();
        }
    }
    return 0;
}

int sprite_resetInventory(lua_State * L)
{
    const char *fnName = "sprite_resetInventory";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushboolean(L, false);
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = CGame::getGame().scene();
        CActor & src = scene[objId];
        CInventory *inventory = src.getInventory();
        if (inventory) {
            inventory->reset();
        }
    }
    return 0;
}

int removeFromInventory(lua_State * L)
{
    const char *fnName = "removeFromInventory";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        int protoId = (int) lua_tonumber(L, 1);
        CInventory *inventory = CGame::getGame().getInventory();
        if (inventory) {
            inventory->removeItem(protoId);
        }
    }
    return 0;
}

int sprite_removeItem(lua_State * L)
{
    const char *fnName = "sprite_removeItem";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = CGame::getGame().scene();
        CActor & src = scene[objId];
        int protoId = (int) lua_tonumber(L, 2);
        CInventory *inventory = src.getInventory();
        if (inventory) {
            inventory->removeItem(protoId);
        }
    }
    return 0;
}

int getSkill(lua_State * L)
{
    lua_pushnumber( L, CGame::getGame().getSkill() );
    return 1;
}

int sprite_getHitTest(lua_State * L)
//#getHitTestC
{
    const char *fnName = "sprite_getHitTest";
    // IN:  int objId
    //      aim
    // OUT: bk, fw, ac, flags, player
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        int aim = (int) lua_tonumber(L, 2);

        CGame & game = CGame::getGame();
        CScene & scene = game.scene();
        CActor & sprite = scene[objId];
        CHitData hitData;
        sprite.hitTest(aim, hitData);

        // create a new table
        int table;

        lua_createtable(L, 0, 0);
        table = lua_gettop(L);

        for (int i=0, j=1; i < hitData.bkCount; ++i, ++j) {
            lua_pushnumber(L, hitData.bkClass[i]);
            lua_rawseti(L, table, j);
        }

        lua_createtable(L, 0, 0);
        table = lua_gettop(L);

        for (int i=0, j=1; i < hitData.fwCount; ++i, j+=2) {
            lua_pushnumber(L, hitData.fwClass[i]);
            lua_rawseti(L, table, j );
            lua_pushnumber(L, hitData.fwEntry[i]);
            lua_rawseti(L, table, j + 1);
        }

        lua_createtable(L, 0, 0);
        table = lua_gettop(L);
        for (int i=0, j=1; i < hitData.acCount; ++i, j+=2) {
            lua_pushnumber(L, hitData.acClass[i]);
            lua_rawseti(L, table, j );
            lua_pushnumber(L, hitData.acEntry[i]);
            lua_rawseti(L, table, j + 1);
        }
        lua_pushnumber( L,  hitData.flags);
        lua_pushboolean( L, hitData.player );
        return 5;
    }
}

int getVersion(lua_State * L)
{
    const char *fnName = "getVersion";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        lua_pushnumber( L, -1 );
    } else {

        lua_pushnumber( L, CGameFile::getVersion());
    }
    return 1;
}

int alert(lua_State * L)
{
    const char *fnName = "alert";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
#ifdef USE_QFILE
        QMessageBox::warning(NULL, QString(""),
                     QString(lua_tostring(L, 1)),
                     QMessageBox::Ok );
#endif
    }
    return 0;
}

int sprite_changeTo(lua_State * L)
//#changeTo
{
    const char *fnName = "sprite_changeTo";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
    } else {
        CGame & game = CGame::getGame();
        int id = (int) lua_tonumber(L, 1);
        int protoId = (int) lua_tonumber(L, 2);
        game.scene()[id].changeTo(protoId);
    }

    return 0;
}

int sprite_getExtra(lua_State *L)
//#getExtraC
{
    const char *fnName = "sprite_getExtra";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = game.scene();
        CActor & src = scene[objId];
        // create a new table
        lua_createtable(L, 0, 0);
        int table = lua_gettop(L);
        int *extra;
        int *index;
        int size;
        src.getExtra(extra,index, size);
        for (int i=0; i < size; ++i) {
            lua_pushnumber(L, extra[i]);
            lua_rawseti(L, table, index[i]);
        }
        delete [] extra;
        delete [] index;
        return 1;
    }
}

int proto_get(lua_State *L)
//#getProtoC
{
    const char *fnName = "proto_get";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int id = (int) lua_tonumber(L, 1);
        CProto & src = game.m_arrProto[id];
        // create a new table
        lua_createtable(L, 0, 0);
        int table = lua_gettop(L);
        typedef struct
        {
            int index;
            int value;
        } PROTOVALUE;
        const PROTOVALUE items [] = {
            { CProto::PPARAM_CLASS       , src.m_nClass },
            { CProto::PPARAM_NO_SMART_MAP, src.m_bNoSmartMap },
            { CProto::PPARAM_JUMP_MODE   , src.m_nJumpMode },
            { CProto::PPARAM_MAX_FALL    , src.m_nMaxFall },

            { CProto::PPARAM_FRAMESET    , src.m_nFrameSet },
            { CProto::PPARAM_FRAMENO     , src.m_nFrameNo },

            { CProto::PPARAM_MOVE_SPEED    , src.m_nMoveSpeed },

            { CProto::PPARAM_ANI_SPEED     , src.m_nAniSpeed },
            { CProto::PPARAM_DEFAULT_AIM   , src.m_nDefaultAim },
            { CProto::PPARAM_POINTS        , src.m_nPoints },

            { CProto::PPARAM_BUDDY         , src.m_nProtoBuddy },
            { CProto::PPARAM_OPTIONS       , src.m_options},
            { CProto::PPARAM_CHPROTO       , src.m_nChProto},
            { CProto::PPARAM_CHSOUND       , src.m_nChSound},

            { CProto::PPARAM_FALLSPEED     , src.m_nFallSpeed},
            { CProto::PPARAM_DAMAGES       , src.m_nDamages },
            { CProto::PPARAM_BONUS_HP      , src.m_nBonusHP },
            { CProto::PPARAM_POWERLEVELS   , src.m_nPowerLevel },

            { CProto::PPARAM_RBDELAY       , src.m_nRbDelay },
            { CProto::PPARAM_REBIRTHS      , src.m_nRebirths },

            { CProto::PPARAM_A_PROTO       , src.m_nAutoProto},
            { CProto::PPARAM_A_BULLET      , src.m_nAutoBullet},
            { CProto::PPARAM_A_SOUND       , src.m_nAutoSound},
            { CProto::PPARAM_A_TRIGGER     , src.m_bAutoTrigger},
            { CProto::PPARAM_HP            , src.m_nHP},

            { CProto::PPARAM_A_PROTO_TIME  , src.m_nAutoProtoTime},
            { CProto::PPARAM_A_BULLET_TIME , src.m_nAutoBulletTime},
            { CProto::PPARAM_A_SOUND_TIME  , src.m_nAutoSoundTime},
            { CProto::PPARAM_A_TRIGGER_TIME, src.m_nAutoTriggerTime},

            { CProto::PPARAM_MAX_BULLETS   , src.m_nMaxBullets },
            { CProto::PPARAM_FIRE_RATES    , src.m_nFireRate },
            { CProto::PPARAM_EXTRA1        , src.m_extra[0] },
            { CProto::PPARAM_EXTRA2        , src.m_extra[1] },
            { CProto::PPARAM_B_SOUND       , src.m_bulletSound},
            { 0,0 },
        };
        lua_pushstring(L, src.m_szName);
        lua_rawseti(L, table, 1);
        for (int i=0; items[i].index; ++i) {
            lua_pushnumber(L, items[i].value);
            lua_rawseti(L, table, items[i].index);
        }
        return 1;
    }
}

int sprite_getUID(lua_State *L)
//#getExtraUID
{
    const char *fnName = "sprite_getUID";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
        return 1;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = game.scene();
        CActor & src = scene[objId];
        lua_pushnumber(L, src.m_string);
        return 1;
    }
}

int getPlayerID(lua_State *L)
//#getPlayerC
{
    const char *fnName = "getPlayerID";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        lua_pushnumber(L, -1);
        return 0;
    } else {
        lua_pushnumber(L, CGame::getGame().svar("playerEntry"));
        return 1;
    }
}

int sprite_isHidden(lua_State *L)
//#isHidden
{
    const char *fnName = "sprite_isHidden";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = game.scene();
        lua_pushboolean(L, scene[objId].isHidden());
        return 1;
    }
}

int sprite_isDead(lua_State *L)
//#isDead
{
    const char *fnName = "sprite_isDead";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = CGame::getGame().scene();
        lua_pushboolean(L, scene[objId].isDead());
        return 1;
    }
}

int sprite_land(lua_State *L)
//#land
{
    const char *fnName = "sprite_land";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);        
        game.scene()[objId].land();
        return 0;
    }
}

int sprite_triggerHitState(lua_State *L)
//#triggerHitState
{
    const char *fnName = "sprite_triggerHitState";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        game.scene()[objId].triggerHitState();
        return 0;
    }
}

int sprite_tryPath(lua_State *L)
//#tryPath
{
    const char *fnName = "sprite_tryPath";
    int argc = lua_gettop(L);
    if (argc < 2 || argc > 3) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        int path = (int) lua_tonumber(L, 2);
        CScene & scene = game.scene();
        bool result;
        if (argc == 2) {
            result = scene[objId].tryPath(path, -1);
        } else {
            int aim = (int) lua_tonumber(L, 3);
            result = scene[objId].tryPath(path, aim);
        }
        lua_pushboolean(L, result);
        return 1;
    }
}

int sprite_stopAnimation(lua_State *L)
//#stopAnimation
{
    const char *fnName = "sprite_stopAnimation";
    int argc = lua_gettop(L);
    if (argc !=1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        game.scene()[objId].stopAnimation();
        return 0;
    }
}

int sprite_isMonster(lua_State *L)
//#isMonster
{
    const char *fnName = "sprite_isMonster";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = game.scene();
        lua_pushboolean(L, scene[objId].isMonster());
        return 1;
    }
}

int sprite_kill(lua_State *L)
//#killSprite
{
    const char *fnName = "sprite_kill";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        game.scene()[objId].kill();
        return 0;
    }
}

int sprite_spawn(lua_State *L)
//#spawn
{
    const char *fnName = "sprite_spawn";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        game.scene()[objId].spawn();
        return 0;
    }
}

int sprite_callEvent(lua_State *L)
//#callObjEvent
{
    const char *fnName = "sprite_callEvent";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        int event = (int) lua_tonumber(L, 2);
        game.scene()[objId].callEvent(event);
        return 0;
    }
}

int triggerPlayerHitState(lua_State *L)
{
    const char *fnName = "triggerPlayerHitState";
    CGame::getGame().triggerPlayerHitState();
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
    }
    return 0;
}

int openStream(lua_State *L)
{
    const char *fnName = "openStream";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushboolean(L, false);
    } else {
        CGame & game = CGame::getGame();
        lua_pushboolean(L,CGame::getGame().music()->open((game.m_path + std::string(lua_tostring(L, 1))).c_str()));
    }
    return 1;
}

int playStream(lua_State *L)
{
    const char *fnName = "playStream";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
    } else {
        CGame::getGame().music()->play();
    }
    return 0;
}

int stopStream(lua_State *L)
{
    const char *fnName = "stopStream";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
    } else {
        CGame::getGame().music()->stop();
    }
    return 0;
}

int sprite_clear(lua_State *L)
//#clearSprite
{
    const char *fnName = "sprite_clear";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CActor & t = game.scene()[objId];
        t.m_nProto = 0;
        t.m_nFrameSet = 0;
        t.m_nFrameNo = 0;
        t.m_nX = 0;
        t.m_nY = 0;
        return 0;
    }
}

int getImageSize(lua_State *L)
{
    const char *fnName = "getImageSize";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int imageSet = (int) lua_tonumber(L, 1);
        int imageNo = (int) lua_tonumber(L, 2);
        CFrameSet & obl = * game.m_arrFrames [ imageSet ];
        CFrame & frame = * obl[imageNo];
        lua_pushnumber(L, frame.m_nLen);
        lua_pushnumber(L, frame.m_nHei);
        return 2;
    }
}

int MIN(lua_State *L)
{
    const char *fnName = "MIN";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        int a = (int) lua_tonumber(L, 1);
        int b = (int) lua_tonumber(L, 2);
        lua_pushnumber(L, std::min(a, b));
        return 1;
    }
}

int MAX(lua_State *L)
{
    const char *fnName = "MAX";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        int a = (int) lua_tonumber(L, 1);
        int b = (int) lua_tonumber(L, 2);
        lua_pushnumber(L, std::max(a, b));
        return 1;
    }
}

int testKey(lua_State *L)
{
    const char *fnName = "testKey";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int keyCode = (int) lua_tonumber(L, 1);
        if (keyCode >= 0 && keyCode < (int)sizeof(game.m_keys)) {
            lua_pushboolean(L, game.m_keys[keyCode]);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }
}

int getLastKey(lua_State *L)
{
    const char *fnName = "getLastKey";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        lua_pushnumber(L, game.counter("lastKey"));//m_lastKey);
        return 1;
    }
}

int setKey(lua_State *L)
{
    // IN: keyCode
    // IN: value
    // OUT: void

    const char *fnName = "setKey";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int keyCode = (int) lua_tonumber(L, 1);
        if (keyCode >= 0 && keyCode < (int)sizeof(game.m_keys)) {
            int keyCode = (int) lua_tonumber(L, 1);
            int value = (int) lua_tonumber(L, 2);
            game.setKey(keyCode, value);
        }
        return 0;
    }
}

int clearKeys(lua_State *L)
{
    // IN: void
    // OUT: void

    const char *fnName = "clearKeys";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        game.clearKeys();
        return 0;
    }
}

int getClosureEvent(lua_State *L)
{
    CGame & game = CGame::getGame();
    lua_pushnumber(L, game.counter("closureEvent"));
    return 1;
}

int getClosureTime(lua_State *L)
{
    lua_pushnumber(L, CGame::getGame().counter("closureTime"));
    return 1;
}

int getClosure(lua_State *L)
{
    lua_pushnumber(L, CGame::getGame().counter("closureEvent"));
    lua_pushnumber(L, CGame::getGame().counter("closureTime"));
    return 2;
}

int ss_manageTasks(lua_State *L)
{
    Q_UNUSED( L );
    CGame & game = CGame::getGame();
    game.manageTasks();
    return 0;
}

int ss_manageKeyEvents(lua_State *L)
{
    Q_UNUSED( L );
    lua_pushnumber(L, CGame::getGame().manageKeyEvents());
    return 1;
}

int ss_getNextTick(lua_State *L)
{
    CGame & game = CGame::getGame();
    lua_pushnumber(L, game.counter("nextTick"));
    return 1;
}

int ss_getPause(lua_State *L)
{
    CGame & game = CGame::getGame();
    lua_pushboolean(L, game.getPause());
    return 1;
}

int ss_setNextTick(lua_State *L)
{
    const char *fnName = "ss_setNextTick";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        CGame & game = CGame::getGame();
        game.counter("nextTick") = (int) lua_tonumber(L, 1);
    }
    return 0;
}

int ss_animate(lua_State *L)
{
    Q_UNUSED( L );
    CGame::getGame().scene().animate();
    return 0;
}

int ss_doManage(lua_State *L)
{
    Q_UNUSED( L );
    CGame::getGame().scene().doManage();
    return 0;
}

int ss_managePlayer(lua_State *L)
{
    Q_UNUSED( L );
    CGame::getGame().managePlayer();
    return 0;
}

int ss_manageAuto(lua_State *L)
{
    Q_UNUSED( L );
    CGame::getGame().scene().manageAuto();
    return 0;
}

int ss_pollDevice(lua_State *L)
{
    Q_UNUSED( L );
    //CGame::getGame().pollDevice();
    return 0;
}

int ss_clearKeys(lua_State *L)
{
    Q_UNUSED( L );
    CGame::getGame().clearKeys();
    return 0;
}

int setClosure(lua_State *L)
{
    const char *fnName = "setClosure";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        CGame & game = CGame::getGame();
        game.setClosure( (int) lua_tonumber(L, 1));
    }
    return 0;
}

int isLevelEnded(lua_State *L)
{
    CGame & game = CGame::getGame();
    lua_pushboolean(L, game.isLevelEnded());
    return 1;
}

int isEndLevelMeet(lua_State *L)
{
    CGame & game = CGame::getGame();
    lua_pushboolean(L, game.isEndLevelMeet());
    return 1;
}

int getNextSecond(lua_State *L)
{
    CGame & game = CGame::getGame();
    lua_pushnumber(L, game.getNextSecond());
    return 1;
}

int getTimeLeft(lua_State *L)
{
    CGame & game = CGame::getGame();
    lua_pushnumber(L, game.getTimeLeft());
    return 1;
}

int setNextSecond(lua_State *L)
{
    const char *fnName = "setNextSecond";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        CGame & game = CGame::getGame();
        game.setNextSecond( (int) lua_tonumber(L, 1));
    }
    return 0;
}

int decTimeLeft(lua_State *L)
{
    Q_UNUSED( L );
    CGame & game = CGame::getGame();
    game.decTimeLeft();
    return 0;
}

int nextTick(lua_State *L)
{
    Q_UNUSED( L );
    CGame & game = CGame::getGame();
    game.nextTick();
    return 0;
}

int updateHP(lua_State *L)
{
    Q_UNUSED( L );
    CGame & game = CGame::getGame();
    game.updateHP();
    return 0;
}

int callLvEvent(lua_State *L)
//# callLevelEvent
{
    const char *fnName = "callLvEvent";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
    } else {
        CGame & game = CGame::getGame();
        game.callLvEvent( (int) lua_tonumber(L, 1));
    }
    return 0;
}

int sprite_getHeight(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_getHeight";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        CFrameSet *frameSet = game.m_arrFrames[sprite.m_nFrameSet];
        CFrame frame = (*frameSet)[sprite.m_nFrameNo];
        lua_pushinteger(L, frame.m_nHei);
        return 1;
    }
}

int sprite_getWidth(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_getWidth";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        CFrameSet *frameSet = game.m_arrFrames[sprite.m_nFrameSet];
        CFrame frame = (*frameSet)[sprite.m_nFrameNo];
        lua_pushinteger(L, frame.m_nLen);
        return 1;
    }
}

int sprite_isActive(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_isActive";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        lua_pushinteger(L, sprite.isActive());
        return 1;
    }
}

int ss_autoCenter(lua_State *L)
{
    Q_UNUSED(L);
    CGame & game = CGame::getGame();
    CActor & player = game.getPlayer();
    player.autoCenter();
    return 0;
}

int sprite_childCount(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_childCount";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        lua_pushinteger(L, sprite.childCount());
        return 1;
    }
}

int sprite_setOwner(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_setOwner";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        int owner = (int) lua_tonumber(L, 2);
        CActor & sprite = game.scene()[objId];
        sprite.setOwner(owner);
        return 0;
    }
}

int sprite_getString(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_getString";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        CStringTable * table = game.getStringTable();
        const char * s = table->get(sprite.m_string);
        lua_pushstring(L, s);
        return 1;
    }
}

int sprite_getHP(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_getHP";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        lua_pushinteger(L, sprite.getHP());
        return 1;
    }
}

int sprite_setHP(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_setHP";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        sprite.setHP((int) lua_tonumber(L, 2));
        return 0;
    }
}

int sprite_isFrozen(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_isFrozen";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        lua_pushinteger(L, sprite.isFrozen());
        return 1;
    }
}

int sprite_isGoal(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_isGoal";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        lua_pushinteger(L, sprite.isGoal());
        return 1;
    }
}

int sprite_frameCount(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_frameCount";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int objId = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[objId];
        CFrameSet & fs = * game.m_arrFrames [ sprite.m_nFrameSet ];
        lua_pushinteger(L, fs.getSize());
        return 1;
    }
}

int frameSet_getSize(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "frameSet_getSize";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        int frameSet = (int) lua_tonumber(L, 1);
        CFrameSet & fs = * game.m_arrFrames [ frameSet ];
        lua_pushinteger(L, fs.getSize());
        return 1;
    }
}

int counter_get(lua_State *L)
//#counters_get
{
    CGame & game = CGame::getGame();
    const char *fnName = "counter_get";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        const char *s = (const char*) lua_tostring(L, 1);
        int value = game.counter(s);
        lua_pushinteger(L, value);
        return 1;
    }
}

int counter_set(lua_State *L)
//#counters_set
{
    CGame & game = CGame::getGame();
    const char *fnName = "counter_set";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        const char *s = (const char*) lua_tostring(L, 1);
        game.counter(s) = (int) lua_tonumber(L, 2);
        return 0;
    }
}

int counter_inc(lua_State *L)
//#counters_inc
{
    CGame & game = CGame::getGame();
    const char *fnName = "counter_inc";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        const char *s = (const char*) lua_tostring(L, 1);
        int & value = game.counter(s);
        lua_pushinteger(L, ++value);
        return 1;
    }
}

int counter_add(lua_State *L)
//#counters_add
{
    CGame & game = CGame::getGame();
    const char *fnName = "counter_add";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        const char *s = (const char*) lua_tostring(L, 1);
        int var = (int) lua_tonumber(L, 2);
        int & value = game.counter(s);
        int tmp = std::max(value + var,0);
        value = tmp;
        lua_pushinteger(L, value);
        return 1;
    }
}

int counter_dec(lua_State *L)
//#counters_dec
{
    CGame & game = CGame::getGame();
    const char *fnName = "counter_dec";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        const char *s = (const char*) lua_tostring(L, 1);
        int & value = game.counter(s);
        int tmp = std::max(value - 1,0);
        value = tmp;
        lua_pushinteger(L, value);
        return 1;
    }
}

int ss_paint(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "ss_paint";
    int argc = lua_gettop(L);
    if (argc != 5 && argc != 6) {
        CGame::error(fnName, 5);
        return 0;
    } else {
        bool fill = true;
        if (argc == 6) {
            fill = lua_toboolean(L,6);
        }
        int x1 = lua_tointeger(L,1);
        int y1 = lua_tointeger(L,2);
        int x2 = lua_tointeger(L,3);
        int y2 = lua_tointeger(L,4);
        unsigned int rgba = (unsigned int)lua_tointeger(L,5);
        game.graphics()->ss_paint(x1,y1,x2,y2,rgba, fill);
        return 0;
    }
}

int ss_paintImage(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "ss_paintImage";
    int argc = lua_gettop(L);
    if (argc != 4) {
        CGame::error(fnName, 4);
        return 0;
    } else {
        int x1 = lua_tointeger(L,1);
        int y1 = lua_tointeger(L,2);
        int frameSet = lua_tointeger(L,3);
        int frameNo = lua_tointeger(L,4);
        game.graphics()->ss_paintImage(x1,y1,frameSet,frameNo);
        return 0;
    }
}

int display_sizeText(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "display_sizeText";
    int argc = lua_gettop(L);
    if (argc != 1 && argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        int id = lua_tointeger(L,1);
        int size;
        if (argc != 2) {
            size = game.displays()->display_sizeText(id);
        } else {
            const char *text = lua_tostring(L,2);
            size = game.displays()->display_sizeText(id, text);
        }
        lua_pushinteger(L, size);
        return 1;
    }
}

int getScreenSize(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "getScreenSize";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        return 0;
    } else {
        int len;
        int hei;
        game.graphics()->getScreenSize(len, hei);
        lua_pushinteger(L, len);
        lua_pushinteger(L, hei);
        return 2;
    }
}

int ss_drawText(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "ss_drawText";
    int argc = lua_gettop(L);
    if (argc != 6 && argc != 7 && argc != 8) {
        CGame::error(fnName, 6);
        return 0;
    } else {
        int x = lua_tointeger(L,1);
        int y = lua_tointeger(L,2);
        const char *text = lua_tostring(L,3);
        int fontid = lua_tointeger(L,4);
        int fontSize = lua_tointeger(L,5);
        unsigned int rgba = (unsigned int)lua_tointeger(L,6);
        int shadowOffset=0;
        unsigned int shadowColor=0;
        if (argc == 8) {
            shadowOffset = (unsigned int)lua_tointeger(L,7);
            shadowColor = (unsigned int)lua_tointeger(L,8);
        }
        game.displays()->drawText(x,y,text,fontid, fontSize, rgba, shadowOffset, shadowColor);
        return 0;
    }
}

int setBorderColor(lua_State *L)
{
    const char *fnName = "setBorderColor";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
    } else {
        CGame & game = CGame::getGame();
        int red =  ((int)lua_tonumber(L, 1)) & 0xff;
        int green =  ((int)lua_tonumber(L, 2)) & 0xff;
        int blue =  ((int)lua_tonumber(L, 3)) & 0xff;
        UINT32 color = red + (green << 8) + (blue << 16) + 0xff000000;
        game.var("borderColor") = color;
    }
    return 0;
}

int ss_clear(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "ss_clear";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
        return 0;
    } else {
        int red =  ((int)lua_tonumber(L, 1)) & 0xff;
        int green =  ((int)lua_tonumber(L, 2)) & 0xff;
        int blue =  ((int)lua_tonumber(L, 3)) & 0xff;
        game.graphics()->clear(red,green,blue);
        return 0;
    }
}

int updateJoyState(lua_State *L)
{
    Q_UNUSED( L );
    CGame & game = CGame::getGame();
    game.updateJoyState();
    return 0;
}

int sprite_set(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_set";
    int argc = lua_gettop(L);
    if (argc != 3) {
        CGame::error(fnName, 3);
        return 0;
    } else {
        int id = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[id];
        if (lua_isnumber(L,2)) {
            int i = static_cast<int>(lua_tointeger(L,2));
            sprite.set(i, ((int)lua_tonumber(L, 3)));
            return 0;
        }
        if (lua_isstring(L,2)) {
            const char *name = lua_tostring(L, 2);
            sprite.set(name, ((int)lua_tonumber(L, 3)));
        }
        return 0;
    }
}

int sprite_get(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_get";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        int id = (int) lua_tonumber(L, 1);
        CActor & sprite = game.scene()[id];
        if (lua_isnumber(L,2)) {
            int i = static_cast<int>(lua_tointeger(L,2));
            lua_pushinteger(L,sprite.get(i));
            return 1;
        }
        if (lua_isstring(L,2)) {
            const char *name = lua_tostring(L, 2);
            lua_pushinteger(L,sprite.get(name));
        }
        return 1;
    }
}

int sprite_unmarkAsGoal(lua_State *L)
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("sprite_unmarkAsGoal", 1);
    } else {
        CScene & scene = game.scene();
        CActor & entry = scene [ (int) lua_tonumber(L, 1) ];
        entry.m_nTriggerKey -=
                (entry.m_nTriggerKey & CGame::TRIGGER_GOAL);
    }
    return 0;
}

int callGameEvent(lua_State *L)
{
    CGame & game = CGame::getGame();
    int argc = lua_gettop(L);
    if ( argc != 1)  {
        CGame::error("callGameEvent", 1);
    } else {
        game.callGameEvent((int) lua_tonumber(L, 1));
    }
    return 0;
}

int var_get(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "var_get";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        const char *s = static_cast<const char*>(lua_tostring(L, 1));
        int value = game.var(s);
        lua_pushinteger(L, value);
        return 1;
    }
}

int var_del(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "var_del";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        const char *s = static_cast<const char*>(lua_tostring(L, 1));
        game.var_del(s);
        return 0;
    }
}

int var_set(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "var_set";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        const char *s = static_cast<const char*>(lua_tostring(L, 1));
        game.var(s) = static_cast<unsigned long long>(lua_tonumber(L, 2));
        return 0;
    }
}

int sprite_isPlayer(lua_State *L)
{
    const char *fnName = "sprite_isPlayer";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int objId = (int) lua_tonumber(L, 1);
        CScene & scene = game.scene();
        lua_pushboolean(L, scene[objId].isMonster());
        return 1;
    }
}

int display_count(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "display_count";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        return 0;
    }
    lua_pushnumber(L, game.displays()->getSize());
    return 1;
}

int sprite_togglePathPlayback(lua_State *L)
{
    CGame & game = CGame::getGame();
    const char *fnName = "sprite_togglePathPlayback";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        int objId = static_cast<int>(lua_tonumber(L, 1));
        CActor & sprite = game.scene()[objId];
        bool enable =  lua_toboolean(L, 2);
        sprite.togglePathPlayback(enable);
        return 0;
    }
}

int warpTo(lua_State *L)
{
    const char *fnName = "warpTo";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        int id = CGame::INVALID;
        if (lua_isstring(L, 1)) {
            std::string uuid = static_cast<const char*>(lua_tostring(L, 1));
            id = game.getLevelByUUID(uuid.c_str());
        }
        if (lua_isnumber(L, 1)) {
            id = static_cast<int>(lua_tonumber(L, 1));
        }

        if (id != CGame::INVALID) {
            game.setClosure( CGame::EVENT_LEVEL_COMPLETED );
            game.var("WarpTo") = id;
        }

        lua_pushnumber(L, id);
        return 1;
    }
}

int saveGame(lua_State *L)
{
    const char *fnName = "saveGame";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        bool result = false;
        if (lua_isstring(L, 1)) {
            std::string filename = static_cast<const char*>(lua_tostring(L, 1));
            CFileWrap file;
            char fname[filename.length()+1];
            strcpy(fname, filename.c_str());
            //qDebug(fname);
            if (file.open(fname, "wb")) {
                game.saveGame(file);
                file.close();
                result = true;
            }
        }
        lua_pushboolean(L,result);
        return 1;
    }
}

int loadGame(lua_State *L)
{
    const char *fnName = "loadGame";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        bool result = false;
        if (lua_isstring(L, 1)) {
            std::string filename = static_cast<const char*>(lua_tostring(L, 1));
            CFileWrap file;
            char fname[filename.length()+1];
            strcpy(fname, filename.c_str());
         //   qDebug(fname);
            if (file.open(fname, "rb")) {
                game.loadGame(file);
                file.close();
                result = true;
            }
        }
        lua_pushboolean(L,result);
        return 1;
    }
}

int strv_set(lua_State *L)
{
    const char *fnName = "strv_set";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 2);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        const char* key = static_cast<const char*>(lua_tostring(L, 1));
        const char* val = static_cast<const char*>(lua_tostring(L, 2));
        game.strv(key) = val;
        return 0;
    }
}

int strv_get(lua_State *L)
{
    const char *fnName = "strv_get";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        const char* key = static_cast<const char*>(lua_tostring(L, 1));
        lua_pushstring(L, game.strv(key).c_str());
        return 1;
    }
}

int strv_del(lua_State *L)
{
    const char *fnName = "strv_del";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        const char* key = static_cast<const char*>(lua_tostring(L, 1));
        game.strv_del(key);
        return 0;
    }
}

int snapshot_take(lua_State *L)
{
    const char *fnName = "snapshot_take";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        game.snapshot().take(game);
        return 0;
    }
}

int snapshot_reload(lua_State *L)
{
    const char *fnName = "snapshot_reload";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        bool result = game.snapshot().reload(game);
        lua_pushboolean(L,result);
        return 1;
    }
}

int remap(lua_State *L)
{
    const char *fnName = "remap";
    int argc = lua_gettop(L);
    if (argc != 0) {
        CGame::error(fnName, 0);
        return 0;
    } else {
        CGame & game = CGame::getGame();
        game.remap();
        return 0;
    }
}

int layer_getOffsetX(lua_State *L)
{
    const char *fnName = "layer_getOffsetX";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
    } else {
        int id =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((id < 0) || (id >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, id);
            CGame::debug(tmp);
            lua_pushnumber(L, -1);
        } else {
            int mx, my;
            (*layers)[id].getOffset(mx, my);
            lua_pushnumber(L, mx);
        }
    }
    return 1;
}

int layer_getOffsetY(lua_State *L)
{
    const char *fnName = "layer_getOffsetY";
    int argc = lua_gettop(L);
    if (argc != 1) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
    } else {
        int id =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((id < 0) || (id >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, id);
            CGame::debug(tmp);
            lua_pushnumber(L, -1);
        } else {
            int mx, my;
            (*layers)[id].getOffset(mx, my);
            lua_pushnumber(L, my);
        }
    }
    return 1;
}

int layer_setOffsetX(lua_State *L)
{
    const char *fnName = "layer_setOffsetX";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
    } else {
        int id =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((id < 0) || (id >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, id);
            CGame::debug(tmp);
        } else {
            int mx, my;
            (*layers)[id].getOffset(mx, my);
            (*layers)[id].setOffset(lua_tonumber(L, 2), my);
        }
    }
    return 0;
}

int layer_setOffsetY(lua_State *L)
{
    const char *fnName = "layer_setOffsetY";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
    } else {
        int id =  lua_tonumber(L, 1);
        CLevel * layers = CGame::getGame().getLayers();
        if ((id < 0) || (id >= layers->getSize())) {
            char tmp[2048];
            sprintf(tmp, "%s(...) - layerId `%d` is out of bound", fnName, id);
            CGame::debug(tmp);
        } else {
            int mx, my;
            (*layers)[id].getOffset(mx, my);
            (*layers)[id].setOffset(mx, lua_tonumber(L, 2));
        }
    }
    return 0;
}


int SHR(lua_State *L)
{
    const char *fnName = "SHR";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
        return 0;
    } else {
        int x =  lua_tonumber(L, 1);
        int y =  lua_tonumber(L, 2);
        lua_pushnumber(L, x >> y);
        return 1;
    }
}


int SHL(lua_State *L)
{
    const char *fnName = "SHL";
    int argc = lua_gettop(L);
    if (argc != 2) {
        CGame::error(fnName, 1);
        lua_pushnumber(L, -1);
        return 0;
    } else {
        int x =  lua_tonumber(L, 1);
        int y =  lua_tonumber(L, 2);
        lua_pushnumber(L, x << y);
        return 1;
    }
}
