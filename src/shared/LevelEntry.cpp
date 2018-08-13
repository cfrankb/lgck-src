/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2014  Francois Blanchette

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
#include <string>
#include <cstring>
#include "Level.h"
#include "LevelEntry.h"
#include "Proto.h"
#include "../shared/IFile.h"

/////////////////////////////////////////////////////////////////////////////
// CLevelEntry

CLevelEntry::CLevelEntry()
{
    memset (this, 0, sizeof (CLevelEntry));
}

CLevelEntry::~CLevelEntry()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLevelEntry message handlers

CLevelEntry & CLevelEntry::operator = (const CLevelEntry & src)
{
    memcpy (this, &src, sizeof (CLevelEntry));
    return *this;
}

void CLevelEntry::moveBy (int tX, int tY)
{
    m_nX += tX;
    m_nY += tY;
}

CLevelEntry::CLevelEntry(int imageSet, int imageNo, int x, int y, bool show)
{
    memset (this, 0, sizeof (CLevelEntry));
    m_nFrameNo = imageNo;
    m_nFrameSet = imageSet;
    m_nX = x;
    m_nY = y;
    if (!show) {
        m_nTriggerKey = 0x80;
    }
}

CLevelEntry::CLevelEntry (int x, int y)
{
    memset (this, 0, sizeof (CLevelEntry));
    m_nX = x;
    m_nY = y;
}

void CLevelEntry::moveTo (int x, int y)
{
    m_nX = x;
    m_nY = y;
}

void CLevelEntry::moveTo (CLevelEntry &entry)
{
    m_nX = entry.m_nX;
    m_nY = entry.m_nY;
}

void CLevelEntry::read (IFile & file)
{
    file.read (this, sizeof (CLevelEntry));
}

CLevelEntry::CLevelEntry (const CLevelEntry & entry)
{
    *this = entry;
}

CLevelEntry::CLevelEntry (const CProto &proto)
{
    memset (this, 0, sizeof (CLevelEntry));
    m_nFrameSet = proto.m_nFrameSet;
    m_nFrameNo = proto.m_nFrameNo;
}

void CLevelEntry::write (IFile &file)
{
    file.write (this, sizeof (CLevelEntry));
}

bool CLevelEntry::operator != (CLevelEntry & s)
{
    return memcmp(this, &s, sizeof(CLevelEntry));
}

bool CLevelEntry::operator == (CLevelEntry & s)
{
    return !memcmp(this, &s, sizeof(CLevelEntry));
}

void CLevelEntry::markAsGoal()
{
    m_nTriggerKey |= TRIGGER_GOAL;
}

void CLevelEntry::unMarkAsGoal()
{
      m_nTriggerKey -= (m_nTriggerKey & TRIGGER_GOAL);
}

void CLevelEntry::show()
{
    m_nTriggerKey &= (-1 ^ TRIGGER_HIDDEN);;
}

void  CLevelEntry::hide()
{
    m_nTriggerKey |= TRIGGER_HIDDEN;
}

void  CLevelEntry::freeze()
{
    m_nTriggerKey |= TRIGGER_FROZEN;
}

void  CLevelEntry::unFreeze()
{
    m_nTriggerKey &= (-1 ^ TRIGGER_FROZEN);
}

void CLevelEntry::setTriggerKey(int key)
{
    key = TRIGGER_KEYS & key ;
    m_nTriggerKey = key | (m_nTriggerKey & ( 0xff ^ TRIGGER_KEYS));
}

int CLevelEntry::triggerKey()
{
    return m_nTriggerKey & TRIGGER_KEYS;
}
