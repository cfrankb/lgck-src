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
#include "../shared/ProtoIndex.h"
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

/////////////////////////////////////////////////////////////////////
// CProtoIndex

std::string CProtoIndex::m_arrFilters[] = {
    "All Sprites",
    "Background",
    "Objects",
    "Player",
    "Monsters",
    "Bullets"
};

const char *CProtoIndex::getFilterName(int i)
{
    return m_arrFilters[i].c_str();
}

int CProtoIndex::getFilterCount()
{
    return sizeof(m_arrFilters)/sizeof(std::string);
}

CProtoIndex::CProtoIndex(CProtoArray *parent, int custom)
{
    m_protoArray = parent;
    m_index = nullptr;
    m_size = 0;
    m_custom = custom;
    m_search = "";
}

void CProtoIndex::forget()
{
    if (m_index) {
        delete m_index;
        m_index = nullptr;
    }
    m_size = 0;
}

CProtoIndex::~CProtoIndex()
{
    forget();
}

bool CProtoIndex::isAccepted(int protoId)
{
    CProto & proto = (*m_protoArray) [protoId];
    int pClass =  proto.m_nClass ;

    switch (m_custom) {
    case FILTER_NONE:
        return protoId != 0;

    case FILTER_BACKGROUND:
        return protoId != 0 && pClass < 0x10;

    case FILTER_OBJECTS:
        return pClass >= 0x10 && pClass < 0x1f;

    case FILTER_PLAYER:
        return pClass == 0x1f;

    case FILTER_MONSTER:
        return pClass >= 0x20 && pClass <= 0x40 ;

    case FILTER_BULLET:
        return pClass == CLASS_PLAYER_BULLET
                || pClass == CLASS_CREATURE_BULLET;
    }

    return false;
}

void CProtoIndex::init()
{
    forget();
    m_index = new int [ m_protoArray->getSize() ] ;
    for (int j = 0; j < m_protoArray->getSize(); ++j) {
        if (isAccepted(j) && matchString(j)) {
            insert(j);
        }
    }
}

// find the insert position for a new proto
int CProtoIndex::findPos(int protoId)
{
    int i = 0;
    int min = 0;
    int max = m_size - 1;
    const char *newName = m_protoArray->getObject(protoId).proto().getName();
    while (max >= min) {
        i = min + (max - min) / 2;
        int result = strcasecmp( m_protoArray->getObject(m_index[i]).proto().getName(), newName );
        if (result <= 0) {
            min = i + 1;
        } else {
            max = i - 1;
        }
    }

    if (m_size && (strcasecmp( m_protoArray->getObject(m_index[i]).proto().getName(), newName) <= 0)) {
        ++i;
    }
    return i;
}

// find the position for a given protoId
int CProtoIndex::findProto (int protoId)
{
    for (int i=0; i< m_size; ++i) {
        if (m_index[i]==protoId) {
            return i;
        }
    }
    return -1;
}

void CProtoIndex::removeIndex (int pos )
{
    for (int k = pos ; k < m_size - 1; ++k) {
        m_index[k] = m_index[k + 1];
    }
    --m_size;
}

void CProtoIndex::removeFromIndex (int protoId)
{
    for (int k=0; k < m_size; ++k) {
        if (m_index[k] > protoId) {
            -- m_index[k];
        } else {
            if (m_index[k] == protoId) {
                removeIndex ( k );
                --k;
            }
        }
    }
}

int CProtoIndex::insert(int protoId)
{
    int i = findPos( protoId );
    for (int k = m_size ; k > i; --k) {
        m_index[k] = m_index[k - 1];
    }
    m_index[i] = protoId;
    ++m_size;

    return i;
}

void CProtoIndex::resizeIndex(int newSize)
{
    int *t = new int [ newSize ] ;

    for (int i = 0; i < m_size; ++i) {
        t[i] = m_index[i] ;
    }

    delete [] m_index;
    m_index = t;
}

int CProtoIndex::getSize()
{
    return m_size;
}

int CProtoIndex::operator [] (int i)
{
    return m_index[i];
}

void CProtoIndex::setTextFilter(const char *search)
{
    std::string tmp = search;
    transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    m_search = tmp;
}

bool CProtoIndex::matchString(int protoId) {
    if (m_search.length() != 0) {
        CProto & proto = (*m_protoArray) [protoId];
        std::string tmp = proto.m_szName;
        transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
        return tmp.find(m_search) != std::string::npos;
    }
    return true;
}


