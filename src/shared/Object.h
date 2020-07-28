/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2013  Francois Blanchette

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

#ifndef __COBJECT_H
#define __COBJECT_H

class IFile;
class CAnimation;
class CPath;

#include "Proto.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////
// CObject

class CObject
{

public:
    CObject();
    ~CObject();
    CObject & operator = (CObject &);
    bool operator != (CObject &);
    bool operator == (CObject &);
    inline CProto & proto() {
        return m_proto;
    }
    const char* getEvent(int i);
    void setEvent(int i, const char *s);
    void resizeEventList(int size);
    int getEventCount();
    static int getAnimationCount();
    static const char * getAnimationName(int i);
    CAnimation & getAnimation (int i) const;
    void setAnimation (int i, const CAnimation & s);
    static int getPathCount();
    static const char * getPathName(int i);
    CPath & getPath(int i) const;
    void clearAnimations();
    void readAnimations(IFile &file);
    void writeAnimations(IFile & file);
    void readPaths(IFile & file);
    void writePaths(IFile & file);
    enum {
        // Animation Sequences
        AS_DEFAULT      = 0x00,
        AS_IDLE         = 0x01,
        AS_CUSTOM1      = 0x02,
        AS_CUSTOM2      = 0x03,
        AS_STAND        = 0x04,
        AS_STAND_UP     = 0x04,
        AS_STAND_DOWN   = 0x05,
        AS_STAND_LEFT   = 0x06,
        AS_STAND_RIGHT  = 0x07,
        AS_MOVE         = 0x08,
        AS_MOVE_UP      = 0x08,
        AS_MOVE_DOWN    = 0x09,
        AS_MOVE_LEFT    = 0x0a,
        AS_MOVE_RIGHT   = 0x0b,
        AS_JUMP         = 0x0c,
        AS_JUMP_UP      = 0x0c,
        AS_JUMP_DOWN    = 0x0d,
        AS_JUMP_LEFT    = 0x0e,
        AS_JUMP_RIGHT   = 0x0f,
        AS_HURT         = 0x10,
        AS_HURT_UP      = 0x10,
        AS_HURT_DOWN    = 0x11,
        AS_HURT_LEFT    = 0x12,
        AS_HURT_RIGHT   = 0x13,
        AS_ATTACK       = 0x14,
        AS_ATTACK_UP    = 0x14,
        AS_ATTACK_DOWN  = 0x15,
        AS_ATTACK_LEFT  = 0x16,
        AS_ATTACK_RIGHT = 0x17,
        AS_RUN          = 0x18,
        AS_RUN_UP       = 0x18,
        AS_RUN_DOWN     = 0x19,
        AS_RUN_LEFT     = 0x1a,
        AS_RUN_RIGHT    = 0x1b,
        AS_DEAD         = 0x1c,
        AS_DEAD_UP      = 0x1c,
        AS_DEAD_DOWN    = 0x1d,
        AS_DEAD_LEFT    = 0x1e,
        AS_DEAD_RIGHT   = 0x1f,
        AS_MAX_VALUE    = 0x1f,
        // Path sequences
        PS_FILTER       = 0xf8,
        PS_DEFAULT      = 0x00,
        PS_IDLE         = 0x01,
        PS_CUSTOM1      = 0x02,
        PS_CUSTOM2      = 0x03,
        PS_ATTACK       = 0x04,
        PS_ATTACK_UP    = 0x04,
        PS_ATTACK_DOWN  = 0x05,
        PS_ATTACK_LEFT  = 0x06,
        PS_ATTACK_RIGHT = 0x07,
        PS_JUMP         = 0x08,
        PS_JUMP_UP      = 0x08,
        PS_JUMP_DOWN    = 0x09,
        PS_JUMP_LEFT    = 0x0a,
        PS_JUMP_RIGHT   = 0x0b,
        PS_JUMP_UP_LEFT = 0x0c,
        PS_JUMP_UP_RIGHT= 0x0d,
        PS_JUMP_DN_LEFT = 0x0e,
        PS_JUMP_DN_RIGHT= 0x0f,
        PS_MOVE         = 0x10,
        PS_MOVE_UP      = 0x10,
        PS_MOVE_DOWN    = 0x11,
        PS_MOVE_LEFT    = 0x12,
        PS_MOVE_RIGHT   = 0x13,
        PS_MOVE_UP_LEFT = 0x14,
        PS_MOVE_UP_RIGHT= 0x15,
        PS_MOVE_DN_LEFT = 0x16,
        PS_MOVE_DN_RIGHT= 0x17,
        PS_SPRITE_CUSTOM= 0x7f,
        // Animation sequence options
        ASO_REPETE      = 0x0001,
        ASO_OPTION2     = 0x0002
    };
    // Events
    enum {
        EO_SPAWN,   //0
        EO_ACTIVATE,
        EO_DEATH,
        EO_HURT,    //3
        EO_TOUCH,
        EO_TRIGGER,
        EO_HANDLER, // 6
        EO_SPLAT,
        EO_HIT_TEST,
        EO_ZKEY,    // 9
        EO_AUTO,
        EO_JUMP,
        EO_FALL,    // 12
        EO_LAND,
        EO_LEAP,
        EO_MOVE,    // 15
        EO_FIRE,
        EO_ATTACK,
        EO_LEFTCLICK,
        EO_RIGHTCLICK,
        EO_NOTIFYCLOSURE
    } EVENT_OBJECT;

    void debug();
private:
    static std::string m_animationList[];
    static std::string m_pathList[];
protected:
    CProto m_proto;
    std::string *m_events;
    CAnimation *m_animations;
    CPath *m_paths;
    int m_eventCount;
};

#endif
