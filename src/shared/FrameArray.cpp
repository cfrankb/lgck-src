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

// FrameArray.cpp : implementation file
//

#include "stdafx.h"
#include <cstring>
#include "FrameArray.h"
#include "LevelEntry.h"

#define GROWBY 16
#define SIZEOF (sizeof (CFrameSet *))

/////////////////////////////////////////////////////////////////////////////
// CFrameArray

CFrameArray::CFrameArray()
{
    m_nSize = 0;
    m_nMax = GROWBY;
    m_arrFilters = new CFrameSet* [ m_nMax ];
}

CFrameArray::~CFrameArray()
{
    forget ();
    delete [] m_arrFilters;
}

/////////////////////////////////////////////////////////////////////////////
// CFrameArray message handlers


void CFrameArray::forget()
{
    while ( getSize() )	{
        delete m_arrFilters [0];
        removeAt (0);
    }
}

int CFrameArray::getSize()
{
    return m_nSize;
}

void CFrameArray::removeAt(int n)
{
    int i;
    for (i = n; i < m_nSize - 1; ++i)  {
        m_arrFilters[ i ] = m_arrFilters[ i + 1];
    }

    --m_nSize;
    m_arrFilters[m_nSize] = nullptr;
}

int CFrameArray::add(CFrameSet *pFilter)
{
    if (m_nSize==m_nMax) {
        m_nMax += GROWBY;
        CFrameSet **t = new CFrameSet * [m_nMax];
        for (int i=0; i < m_nSize; ++i) {
            t[i] = m_arrFilters[i];
        }
        delete [] m_arrFilters;
        m_arrFilters = t;
    }

    m_arrFilters[m_nSize] = pFilter;
   // m_arrFilters[m_nSize]->copyTags(*pFilter);
    ++m_nSize;
    return m_nSize - 1;
}

CFrameSet * CFrameArray::operator [] (int n)
{
    return m_arrFilters[n];
}

CFrame * CFrameArray::getFrame (const int imageSet, const int imageNo)
{
    CFrameSet & filter = * m_arrFilters[imageSet];
    return filter [imageNo];
}

CFrame * CFrameArray::getFrame (const CLevelEntry &entry)
{
    CFrameSet & filter = * m_arrFilters[entry.m_nFrameSet];
    return filter [entry.m_nFrameNo];
}

void CFrameArray::setAt (int n, CFrameSet * pFilter)
{
    m_arrFilters [n] = pFilter;
}

int CFrameArray::getNameIndex (const char *str)
{
    int i;
    for (i=0; i<m_nSize; i++) {
        if (strcmp(m_arrFilters[i]->m_name.c_str(), str)==0)
            return i;
    }

    return NOT_FOUND;
}

CFrameSet * CFrameArray::operator [] (const char* name)
{
    int i;
    for (i=0; i< getSize(); i++) {
        if (strcmp(m_arrFilters[i]->m_name.c_str(), name)==0) {
            return m_arrFilters[i];
        }
    }

    return nullptr;
}

 int CFrameArray::indexOfUUID(const char *uuid)
 {
     for (int i=0; i < getSize(); i++) {
         CFrameSet * fs = m_arrFilters[i];
         if (!strcmp(fs->tag("UUID").c_str(), uuid)) {
             return i;
         }
     }
     return NOT_FOUND;
 }
