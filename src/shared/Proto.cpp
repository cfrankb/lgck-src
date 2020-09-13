/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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
#include "ProtoArray.h"
#include "LevelEntry.h"
#include "vlamits3.h"
#include "Game.h"
#include "../shared/IFile.h"
#include "../shared/FileWrap.h"
#include "helper.h"

/////////////////////////////////////////////////////////////////////////////
// CProto

CProto::CProto()
{
    memset(static_cast<void*>(this), 0, sizeof(CProto));
}

CProto::~CProto()
{
}

CProto & CProto::operator = (const CProto proto)
{
    memcpy(static_cast<void*>(this), &proto, sizeof (CProto));
    return *this;
}

bool CProto::operator == (const CProto proto) const
{
    return !memcmp (this, &proto, sizeof (CProto));
}

bool CProto::operator != (const CProto proto) const
{
    return memcmp (this, &proto, sizeof (CProto));
}

CProto::CProto (const CProto & proto)
{
    memcpy(static_cast<void*>(this), &proto, sizeof (CProto));
   // resetUUID();
}

CProto::CProto (const char* s)
{
    memset(static_cast<void*>(this), 0, sizeof(CProto));
    strcpy (m_szName, s);
    resetUUID();
}

void CProto::read(IFile &file, int nSize)
{
    if (!nSize) {
        nSize = sizeof (CProto);
    }

    file.read(this, nSize);
}

void CProto::write(IFile &file)
{
    file.write(this, sizeof (CProto));
}

const char *CProto::getName() const
{
    return m_szName;
}

bool CProto::getOption(int option) const
{
    return (m_options & option) == option;
}

void CProto::setOption(int option, bool set)
{
    if (set) {
        m_options |= option;
    } else {
        m_options -= m_options & option;
    }
}

bool CProto::isStateSolid(int fromAim) const
{
    if (m_solidState & SOLID_OVERIDE) {
        switch (fromAim) {
        case CGame::UP:
            return m_solidState & CProto::SOLID_DOWN;
        case CGame::FALL:
        case CGame::DOWN:
            return m_solidState & CProto::SOLID_UP;
        case CGame::LEFT:
            return m_solidState & CProto::SOLID_RIGHT;
        case CGame::RIGHT:
            return m_solidState & CProto::SOLID_LEFT;
        default:
            return false;
        }
    } else {
        return false;
    }
}

bool CProto::isBkClass() const
{
    return m_nClass < CLASS_ANIMATED_THINGS;
}

bool CProto::isFkClass() const
{
    return m_nClass >= CLASS_ANIMATED_THINGS
            && m_nClass < CLASS_GENERIC_COS;
}

bool CProto::isAcClass() const
{
    return m_nClass >= CLASS_GENERIC_COS;
}

void CProto::resetUUID()
{
    strcpy(m_uuid, getUUID());
}

bool CProto::isPlayer() const
{
    return m_nClass == CLASS_PLAYER_OBJECT;
}

bool CProto::isMonster() const
{
    const int monsterClasses []= {
        CLASS_GENERIC_COS,
        CLASS_GENERIC_COS___COMPLEX,
        CLASS_DRONE_UP_DOWN,
        CLASS_DRONE_LEFT_RIGHT,
        CLASS_RANDOM_ATTACKER,
        CLASS_VAMPIRE_PLANT_SOLID,
        CLASS_WACKER_UP_DOWN,
        CLASS_WACKER_LEFT_RIGHT,
        CLASS_CREATURE_BULLET
    };

    for (unsigned int i=0; i < sizeof(monsterClasses) / sizeof(int); ++i) {
        if (m_nClass == monsterClasses[i]) {
            return true;
        }
    }
    return false;
}
