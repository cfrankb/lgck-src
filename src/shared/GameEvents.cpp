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
#include <string.h>
#include "IFile.h"
#include "GameEvents.h"

std::string CGameEvents::m_eventList[]=
{
  "onInitGame",
  "onPrepareLevel",
  "onCompleteLevel",
  "onDeath",
  "onGoalCollected",
  "onGoalKilled",
  "onPickup",
  "onLoadGame",
  "onSaveGame",
  "onRestartLevel",
  "onNotifyClosure"
};

CGameEvents::CGameEvents()
{
    m_events = new std::string[ sizeof(m_eventList) / sizeof(std::string) ];
}

CGameEvents::~CGameEvents()
{
    delete [] m_events;
    m_events = NULL;
}

bool CGameEvents::read(IFile & file)
{
    int size = 0;
    file.read(&size, 4);

    int max = (int) (sizeof(m_eventList) / sizeof(std::string));
    size = std::min(size, max );

    int i;
    for (i=0; i < size; ++i) {
        file >> m_events[i];
    }

    for (; i < max; ++i) {
        m_events[i] = std::string("");
    }
    return true;
}

void CGameEvents::forget()
{
    int max = (int) (sizeof(m_eventList) / sizeof(std::string));
    for (int i=0; i < max; ++i) {
        m_events[i] = std::string("");
    }
}

bool CGameEvents::write(IFile &file)
{
    int size = sizeof(m_eventList) / sizeof(std::string);
    file.write(&size,4);
    for (int i=0; i < size; ++i) {
        file << m_events[i];
    }
    return true;
}

int CGameEvents::getSize()
{
    return (int) (sizeof(m_eventList) / sizeof(std::string));
}

const char *CGameEvents::getEvent(int i)
{
    return m_events[i].c_str();
}

void CGameEvents::setEvent(int i, const char *s)
{
    m_events[i] = s;
}

const char *CGameEvents::getName(int i)
{
    return m_eventList[i].c_str();
}

const char *CGameEvents::operator[] (int i)
{
    return m_events[i].c_str();
}
