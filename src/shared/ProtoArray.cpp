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

// ProtoArray.cpp : implementation file
//

#include "stdafx.h"
#include <cstring>
#include <string>
#include "ProtoArray.h"
#include "LevelEntry.h"
#include <zlib.h>
#include "vlamits3.h"
#include "Game.h"
#include "../shared/IFile.h"
#include "../shared/FileWrap.h"

/////////////////////////////////////////////////////////////////////////////
// CProtoArray

CProtoArray::CProtoArray()
{
    m_nSize = 0;
    m_nMax = GROWBY;
    m_objects = new CObject [ m_nMax ];

    m_index = NULL;
    m_indexSize = 0;
}

CProtoArray::~CProtoArray()
{
    forget();
    if (m_objects) {
        delete [] m_objects;
    }
}

/////////////////////////////////////////////////////////////////////////////
// static members

std::string CProtoArray::m_eventList[] = {
    "onSpawn",  //0
    "onActivate",
    "onDeath",
    "onHurt",   //3
    "onTouch",
    "onTrigger",
    "onHandler",// 6
    "onSplat",
    "onHitTest",
    "onZKey",   // 9
    "onAuto",
    "onJump",
    "onFall",   // 12
    "onLanding",
    "onLeap",
    "onMove",
    "onFire",
    "onAttack",
    "onLeftClick",
    "onRightClick",
    "onNotifyClosure"
};

CProto CProtoArray::m_protoTmp;

/////////////////////////////////////////////////////////////////////////////
// CProtoArray message handlers

const char *CProtoArray::getEventName(int i)
{
    return m_eventList[i].c_str();
}

bool CProtoArray::read(IFile &file)
{
    INT32 nEntrySize = 0;
    INT32 i = 0;
    INT32 version = 0;

    // clear array
    forget();

    // read file version
    file.read(&i, sizeof(INT32));
    if (i == -1) {
        file.read(&version, sizeof(INT32));
        file.read(&i, sizeof(INT32));
    }

    // read entry size
    file.read(&nEntrySize, sizeof(nEntrySize));
    //qDebug("entrySize:%d [%ld]\n", nEntrySize, sizeof(CProto));
    char *t = new char [ std::max(nEntrySize, (int)sizeof(CProto)) ];
    memset(t, 0, sizeof(CProto));

    CProto* proto = (CProto*)t;
    // all the proto data
    while (i) {
        file.read(t, nEntrySize);
        if (version < 4) {
            // Fix-up this issue
            proto->m_nMaxFall = 0;
        }
        add(*proto);
        --i;
    }

    delete [] t ;

    bool result = false;
    switch (version) {
        case 0:
            // this version doesn't include extended Event data
            result = true;
        break;

        case 1:
        case 2:
        case 3:
        case 4:
            result = readEx(file, version);
        break;

        default:
            qDebug("unknown proto.dat version (0x%.8x)\n", version);
        break;
    }

    return result;
}

bool CProtoArray::readEx(IFile &file, int version)
{
    std::string masterList[getEventCount()];
    for (int i=0; i < getEventCount(); ++i) {
        masterList[i] = m_eventList[i].c_str();
    }

    // read eventCount
    INT32 eventCount = 0;
    file.read(&eventCount, sizeof(INT32));

    std::string eventNames[eventCount];
    // read eventList
    for (int i=0; i < eventCount; ++i) {
        file >> eventNames[i];
    }

    // read extended data
    for (int i=0; i < m_nSize; ++i) {
        m_objects[i].resizeEventList( getEventCount() );
        for (int j=0; j < eventCount; ++j) {
            std::string temp;
            file >> temp;
            for (int k=0; k < getEventCount();++k) {
                if (masterList[k]==eventNames[j].c_str()){
                    m_objects[i].setEvent(k, temp.c_str());
                    break;
                }
            }
        }
    }

    if (version > 1) {
        for (int i=0; i < m_nSize; ++i) {
            m_objects[i].readAnimations(file);
        }
    }

    if (version > 2) {
        for (int i=0; i < m_nSize; ++i) {
            m_objects[i].readPaths(file);
        }
    }
    return true;
}

bool CProtoArray::write(IFile &file)
{
    INT32 nEntrySize = sizeof(CProto);
    INT32 version = PROTO_VERSION;
    INT32 marker = -1;

    file.write(&marker, 4);
    file.write(&version, 4);

    file.write (&m_nSize, 4);
    file.write (&nEntrySize, 4);
    // write sprite prototypes
    for (int i=0; i < m_nSize; ++i)	{
        m_objects[i].proto().write(file);
    }

    // write eventCount
    INT32 eventCount = getEventCount();
    file.write(&eventCount, sizeof(INT32));

    // write eventList
    for (int i=0; i < eventCount; ++i) {
        file << m_eventList[i];
    }

    // write extended Event data
    for (int i=0; i < m_nSize; ++i) {
        for (int j=0; j < eventCount; ++j) {
            std::string event = m_objects[i].getEvent(j);
            file << event;
        }
    }

    // write animation data
    if (PROTO_VERSION > 1) {
        for (int i=0; i < m_nSize; ++i) {
            m_objects[i].writeAnimations(file);
        }

        for (int i=0; i < m_nSize; ++i) {
            m_objects[i].writePaths(file);
        }
    }

    return true;
}

void CProtoArray::forget()
{
    m_nSize = 0;
    forgetIndex();
}

void CProtoArray::add (const CProto proto)
{
    if (m_nSize == m_nMax) {

        m_nMax += GROWBY;
        CObject *t = new CObject[m_nMax];
        for (int i=0; i < m_nSize; i++) {
            t[i] = m_objects[i];
        }

        delete [] m_objects;
        m_objects = t;
    }

    CObject & obj  = m_objects[m_nSize];
    obj.proto() = proto;
    obj.resizeEventList( getEventCount() );

    // clear the animations
    obj.clearAnimations();

    ++m_nSize;
}

void CProtoArray::removeAt (int n)
{
    for (int i = n; i<m_nSize - 1; ++i) {
        m_objects[i] = m_objects[i + 1];
    }
    --m_nSize;
}


CProto & CProtoArray::operator [] (int n)
{
    if ((n < 0) || (n >= m_nSize)) {
        return m_protoTmp;
    } else {
        return m_objects[n].proto();
    }
}

int CProtoArray::getSize()
{
    return m_nSize;
}

int CProtoArray::getEventCount()
{
    return sizeof(m_eventList) / sizeof(std::string);
}

int CProtoArray::getProtoIndex (const char *str)
{
    for (int i = 0; i<m_nSize; ++i) {
        if ( !strcmp(m_objects[i].proto().m_szName, str)) {
            return i;
        }
    }
    return -1;
}

void CProtoArray::killFrameSet (int nFrameSet)
{
    for (int i=0; i < getSize(); i++) {
        if (m_objects[i].proto().m_nFrameSet == nFrameSet) {
            m_objects[i].proto().m_nFrameSet = 0;
            m_objects[i].proto().m_nFrameNo = 0;
        }
        if (m_objects[i].proto().m_nFrameSet > nFrameSet) {
            m_objects[i].proto().m_nFrameSet--;
        }
    }
}

void CProtoArray::killProto (int nProto)
{
    for (int i = 0; i < getSize(); ++i) {
        INT16 * vars [] = {
            & m_objects[i].proto().m_nChProto,
            & m_objects[i].proto().m_nAutoBullet,
            & m_objects[i].proto().m_nAutoProto,
            & m_objects[i].proto().m_nProtoBuddy
        };

        for (int j = 0; j < (int)(sizeof (vars) / sizeof(INT16*)); ++j) {
            if ( *(vars[j]) == nProto ) {
                *(vars[j]) = 0;
            }

            if ( *(vars[j]) > nProto ) {
                -- *(vars[j]);
            }
        }
    }
    removeAt(nProto);
}

void CProtoArray::killSound (int nSound)
{
    for (int i = 0; i < getSize(); ++i) {

        if (m_objects[i].proto().m_nAutoSound == nSound) {
            m_objects[i].proto().m_nChSound = 0;
        }

        if (m_objects[i].proto().m_nAutoSound > nSound) {
            --m_objects[i].proto().m_nChSound = 0;
        }

        if (m_objects[i].proto().m_nChSound == nSound) {
            m_objects[i].proto().m_nChSound = 0;
        }

        if (m_objects[i].proto().m_nChSound > nSound) {
            --m_objects[i].proto().m_nChSound = 0;
        }
    }
}

CProto & CProtoArray::getProto(CLevelEntry & entry)
{
    if (entry.m_nProto < 0 || entry.m_nProto>m_nSize) {
        return m_protoTmp;
    } else {
        return m_objects[entry.m_nProto].proto();
    }
}

CObject & CProtoArray::getObject(int i)
{
    return m_objects[i];
}

/////////////////////////////////////////////////////////////////////////////
// indexing

void CProtoArray::indexArray ()
{
    forgetIndex();
    m_index = new int [ m_nSize ] ;
    for (int j = 0; j < m_nSize; ++j) {
        insertIndex(j);
    }
}

void CProtoArray::forgetIndex ()
{
    m_indexSize = 0;
    if (m_index) {
        delete [] m_index;
        m_index = NULL;
    }
}

int CProtoArray::getAtIndex( int i )
{
    return m_index [ i ];
}

int CProtoArray::findIndexPos( int protoId )
{
    int i = 0;
    int min = 0;
    int max = m_indexSize - 1;
    char *newName = m_objects[protoId].proto().getName();
    while (max >= min) {
        i = min + (max - min) / 2;
        int result = strcasecmp( m_objects[m_index[i]].proto().getName(), newName );
        if (result <= 0) {
            min = i + 1;
        } else {
            max = i - 1;
        }
    }

    if (m_indexSize && (strcasecmp( m_objects[m_index[i]].proto().getName(), newName) <= 0)) {
        ++i;
    }

    return i;
}

void CProtoArray::removeIndex (int pos )
{
    for (int k = pos ; k < m_indexSize - 1; ++k) {
        m_index[k] = m_index[k + 1];
    }
    --m_indexSize;
 }

void CProtoArray::removeFromIndex (int protoId)
{
    for (int k=0; k < m_indexSize; ++k) {
        if (m_index[k] > protoId) {
            -- m_index[k];
        } else {
            if (m_index[k] == protoId) {
                removeIndex ( k );
            }
        }
    }
}

int CProtoArray::insertIndex(int protoId)
{
    int i = findIndexPos( protoId );
    for (int k = m_indexSize ; k > i; --k) {
        m_index[k] = m_index[k - 1];
    }
    m_index[i] = protoId;
    ++m_indexSize;

    return i;
}

void CProtoArray::resizeIndex(int newSize)
{
    int *t = new int [ newSize ] ;

    for (int i = 0; i < m_indexSize; ++i) {
        t[i] = m_index[i] ;
    }

    delete [] m_index;
    m_index = t;
}

bool CProtoArray::isIndexed()
{
    return m_index != NULL;
}

void CProtoArray::debugIndex()
{
    for (int k = 0 ; k < m_indexSize; ++k) {
        qDebug("index %d = %d (%s)\n", k, m_index[k], m_objects[ m_index[k] ].proto().getName());
    }
}

void CProtoArray::debugIndex(CFileWrap & file)
{
    for (int k = 0 ; k < m_indexSize; ++k) {
        const char *format = "index %-3d\t%d \t%s\n";
        char *buf = new char[strlen(format) + 64];
        sprintf(buf, format, k, m_index[k], m_objects[ m_index[k] ].proto().getName());
        file += buf;
    }
}

int CProtoArray::getIndexSize()
{
    return m_indexSize;
}

CProtoIndex * CProtoArray::createIndex(int pattern)
{
    CProtoIndex *index = new CProtoIndex(this, pattern);
    index->init();
    return index;
}


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
    m_index = NULL;
    m_size = 0;
    m_custom = custom;
}

void CProtoIndex::forget()
{
    if (m_index) {
        delete m_index;
        m_index = NULL;
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
        if (isAccepted(j)) {
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
    char *newName = m_protoArray->getObject(protoId).proto().getName();
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

void CProtoIndex::debug()
{
    CFileWrap file;
    file.open("../debug/protoIndex.txt", "a");
    char s[1024];
    for (int k = 0 ; k < m_size; ++k) {
        qDebug("index %d = %d (%s)\n", k, m_index[k], m_protoArray->getObject( m_index[k] ).proto().getName());
        sprintf(s, "index %d = %d (%s)\n", k, m_index[k], m_protoArray->getObject( m_index[k] ).proto().getName());
        file.write(s, strlen(s));
    }

    file += "\n--------------------------------------\n";
    file.close();
}

/////////////////////////////////////////////////////////////////////////////
// CProto

CProto::CProto()
{
    memset (this, 0, sizeof(CProto));
}

CProto::~CProto()
{
}

CProto & CProto::operator = (const CProto proto)
{
    memcpy (this, &proto, sizeof (CProto));
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
    memcpy (this, &proto, sizeof (CProto));
}

CProto::CProto (const char* s)
{
    memset (this, 0, sizeof(CProto));
    strcpy (m_szName, s);
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

char *CProto::getName()
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
