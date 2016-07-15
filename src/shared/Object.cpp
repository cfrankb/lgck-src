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
#include "Object.h"
#include "ProtoArray.h"
#include "Path.h"
#include <zlib.h>
#include <cstring>
#include "../shared/Animation.h"
#include "../shared/IFile.h"

/////////////////////////////////////////////////////////////////////////////
// CObject

CObject::CObject()
{
    m_events = NULL;
    m_eventCount = 0;

    m_animations = new CAnimation[ getAnimationCount()];
    m_paths = new CPath [ getPathCount() ];
}

CObject::~CObject()
{
    if (m_events) {
        delete [] m_events;
    }

    if (m_animations) {
        delete [] m_animations;
    }

    if (m_paths) {
        delete [] m_paths;
    }
}

CObject & CObject::operator = (CObject & s)
{
    //qDebug("CObject::operator =\n");
    m_proto = s.proto();

    resizeEventList( s.getEventCount() );

    for (int i = 0; i < s.getEventCount(); ++i) {
        m_events[i] = s.getEvent(i);
    }

    // TODO: copy the animation sequences too

    int count = getAnimationCount();
    for (int i = 0; i < count; ++i) {
        m_animations[i] = s.getAnimation(i);
    }

    return *this;
}

bool CObject::operator != (CObject & s)
{
    if (memcmp(&m_proto, & s.proto(), sizeof(CProto))) {
        return true;
    }

    if (getEventCount() != s.getEventCount()) {
        return true;
    }

    for (int i=0; i < s.getEventCount(); ++i) {
        if (m_events[i] != s.getEvent(i)) {
            return true;
        }
    }

    // TODO: test the animation sequences too

    int count = getAnimationCount();
    for (int i = 0; i < count; ++i) {
        if (m_animations[i] != s.getAnimation(i)) {
            return true;
        }
    }

    return false;
}

bool CObject::operator == (CObject & s)
{
    return ! (*this != s);
}

const char *CObject::getEvent(int i)
{
    return m_events[i].c_str();
}

void CObject::setEvent(int i, const char * s)
{
    m_events[i] = s;
}

void CObject::resizeEventList( int size )
{
    if (m_events) {
        delete [] m_events;
    }

    m_events = new std::string[size];
    m_eventCount = size;
}

int CObject::getEventCount()
{
    return m_eventCount;
}

/////////////////////////////////////////////////////////////////////
// animations

std::string CObject::m_animationList[] = {
    "Default",      // 0
    "Idle",
    "Custom1",
    "Custom2",
    "Stand Up",     // 4
    "Stand Down",
    "Stand Left",
    "Stand Right",
    "Move Up",      // 8
    "Move Down",
    "Move Left",
    "Move Right",
    "Jump Up",      // c
    "Jump Down",
    "Jump Left",
    "Jump Right",
    "Hurt Up",      // 10
    "Hurt Down",
    "Hurt Left",
    "Hurt Right",
    "Attack Up",    // 14
    "Attack Down",
    "Attack Left",
    "Attack Right",
    "Run Up",       // 18
    "Run Down",
    "Run Left",
    "Run Right",
    "Dead Up",      // 1c
    "Dead Down",
    "Dead Left",
    "Dead Right"
};

const char *CObject::getAnimationName(int i)
{
    return m_animationList[i].c_str();
}

int CObject::getAnimationCount()
{
    return sizeof(m_animationList) / sizeof(std::string);
}

CAnimation & CObject::getAnimation(int i) const
{
    return m_animations[i];
}

void CObject::setAnimation (int i, const CAnimation & s)
{
    m_animations[i] = s;
}


void CObject::readAnimations(IFile & file)
{
    int count = getAnimationCount();
    int j = 0;

    // count of initiated animation sequences
    file.read(&j, sizeof(UINT8));

    for (int i=0; i < j; ++i) {
        m_animations[i].read(file);
    }

    // clear extra animation entries
    for (int i=j; i < count; ++i) {
        m_animations[i].forget();
    }
}

void CObject::writeAnimations(IFile &file)
{
    int count = getAnimationCount();
    int j = 0;

    for (int i=0; i < count; ++i) {
        if (m_animations[i].getSize()) {
            j = i + 1;
        }
    }

    // count of active animation sequences
    file.write(&j, sizeof(UINT8));

    for (int i=0; i < j; ++i) {
        m_animations[i].write(file);
    }
}

void CObject::clearAnimations()
{
    for (int i=0; i < getAnimationCount(); ++i) {
        CAnimation & anime = m_animations[i];
        anime.forget();
    }
}

/////////////////////////////////////////////////////////////////////
// path

std::string CObject::m_pathList[] = {
    "Default",
    "Idle",
    "Custom 1",
    "Custom 2",
    "Attack Up",
    "Attack Down",
    "Attack Left",
    "Attack Right",
    "Jump Up",  // 8
    "Jump Down",
    "Jump Left",
    "Jump Right",
    "Jump Up Left", // c
    "Jump Up Right",
    "Jump Down Left",
    "Jump Down Right",
    "Up",
    "Down",
    "Left",
    "Right",
    "Up Left", //
    "Up Right",
    "Down Left",
    "Down Right"
};

/*    PARAM_JUMP_UP           = 19,
    PARAM_JUMP_DOWN         = 20,
    PARAM_JUMP_LEFT         = 21,
    PARAM_JUMP_RIGHT        = 22,
    PARAM_JUMP_UP_LEFT      = 23,
    PARAM_JUMP_UP_RIGHT     = 24,
    PARAM_JUMP_DOWN_LEFT    = 25,
    PARAM_JUMP_DOWN_RIGHT   = 26,*/

int CObject::getPathCount()
{
    return sizeof(m_pathList)/sizeof(std::string);
}

const char *CObject::getPathName(int i)
{
    return m_pathList[i].c_str();
}

CPath & CObject::getPath(int i) const
{
    return m_paths[i];
}

void CObject::readPaths(IFile &file)
{
    int count = getPathCount();
    int j = 0;

    // count of initiated path sequences
    file.read(&j, sizeof(UINT8));

    for (int i=0; i < j; ++i) {
        m_paths[i].read(file);
    }

    // clear extra path entries
    for (int i=j; i < count; ++i) {
        m_paths[i].forget();
    }
}

void CObject::writePaths(IFile &file)
{
    int count = getPathCount();
    int j = 0;

    for (int i=0; i < count; ++i) {
        if (m_paths[i].getSize()) {
            j = i + 1;
        }
    }

    // count of active paths
    file.write(&j, sizeof(UINT8));

    for (int i=0; i < j; ++i) {
        m_paths[i].write(file);
    }
}
