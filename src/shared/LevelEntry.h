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

#ifndef _LEVELENTRY_H
#define _LEVELENTRY_H

class IFile;
class CProto;

#include "stdafx.h"

// LevelEntry.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLevelEntry 

constexpr int TRIGGER_KEYS    = 0x1f;
constexpr int TRIGGER_GOAL    = 0x20;
constexpr int TRIGGER_FROZEN  = 0x40;
constexpr int TRIGGER_HIDDEN  = 0x80;
constexpr int TRIGGER_MASK    = 0xe0;

class CLevelEntry 
{
    // Construction
public:
    CLevelEntry (int imageSet, int imageNo, int x, int y, bool show);
    CLevelEntry (const CLevelEntry & entry);
    CLevelEntry (const CProto & proto);
    CLevelEntry (int x, int y);
    CLevelEntry();

    // Attributes
public:
    void markAsGoal();
    void unMarkAsGoal();
    void show();
    void hide();
    void freeze();
    void unFreeze();
    void setTriggerKey(int key);
    int triggerKey();

    // Operations
public:
    void moveTo (CLevelEntry &entry);
    void moveTo (int x, int y);
    void moveBy (int byX, int byY);
    CLevelEntry & operator = (const CLevelEntry & src);

    // Implementation
public:
    ~CLevelEntry();
    void read (IFile &file);
    void write (IFile & file);
    bool operator != (CLevelEntry & s);
    bool operator == (CLevelEntry & s);

    uint8_t m_nAim;
    uint8_t m_nStateFlag; // user by player at runtime
    uint16_t m_string;

    int16_t m_nX;
    int16_t m_nY;
    uint16_t m_nFrameSet;
    uint16_t m_nFrameNo;
    uint8_t m_nActionMask; // used by player at runtime
    // 01 normal        10 extra 1
    // 02 nightmare     20 goal
    // 04 hell          40 frozen
    // 08 insane        80 hidden

    uint8_t m_nTriggerKey;
    // 01,02,04,08,10  trigger
    // 20              goal
    // 40              frozen
    // 80              hidden

    int16_t m_nProto;     // sprite type id
    uint16_t m_path;
    uint16_t m_playback;
};

/////////////////////////////////////////////////////////////////////////////

#endif
