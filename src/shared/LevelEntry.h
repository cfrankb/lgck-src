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
#include "Proto.h"

// LevelEntry.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLevelEntry 

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

    UINT8 m_nAim;
    UINT8 m_nStateFlag; // user by player at runtime
    UINT16 m_string;

    INT16 m_nX;
    INT16 m_nY;
    UINT16 m_nFrameSet;
    UINT16 m_nFrameNo;
    UINT8 m_nActionMask; // used by player at runtime
    // 01 normal        10 extra 1
    // 02 nightmare     20 goal
    // 04 hell          40 frozen
    // 08 insane        80 hidden

    UINT8 m_nTriggerKey;
    // 01,02,04,08,10  trigger
    // 20              goal
    // 40              frozen
    // 80              hidden

    INT16 m_nProto;     // sprite type id
    UINT16 m_path;
    UINT16 m_playback;
};

/////////////////////////////////////////////////////////////////////////////

#endif
