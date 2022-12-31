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

// SndArray.cpp : implementation file
//

#include "SndArray.h"
#include "Snd.h"
#include <cstring>
#include <cstdlib>
#include "IFile.h"

/////////////////////////////////////////////////////////////////////////////
// CSndArray

CSndArray::CSndArray()
{
    m_nMax = GROWBY;
    m_nSize = 0;
    m_arrSnds = new CSnd *[m_nMax];
}

CSndArray::~CSndArray()
{
    forget();
    delete[] m_arrSnds;
}

/////////////////////////////////////////////////////////////////////////////
// CSndArray message handlers

int CSndArray::getSize()
{
    return m_nSize;
}

void CSndArray::forget()
{
    for (int i = 0; i < m_nSize; ++i)
    {
        if (m_arrSnds[i])
        {
            delete m_arrSnds[i];
            m_arrSnds[i] = nullptr;
        }
    }
    m_nSize = 0;
}

void CSndArray::removeAt(int n)
{
    // TODO: check this function again
    for (int i = n; i < m_nSize - 1; i++)
    {
        m_arrSnds[i] = m_arrSnds[i + 1];
    }

    --m_nSize;
    m_arrSnds[m_nSize] = nullptr;
}

void CSndArray::add(CSnd *pSnd)
{
    if (m_nSize == m_nMax)
    {
        m_nMax += GROWBY;
        CSnd **t = new CSnd *[m_nMax];
        for (int i = 0; i < m_nSize; i++)
        {
            t[i] = m_arrSnds[i];
        }

        for (int i = 0; i < m_nSize; i++)
        {
            m_arrSnds[i] = nullptr;
        }
        delete[] m_arrSnds;
        m_arrSnds = t;
    }

    m_arrSnds[m_nSize] = pSnd;
    m_nSize++;
}

CSnd *CSndArray::operator[](int n)
{
    if (n < 0 || n >= m_nSize)
    {
        return nullptr;
    }
    else
    {
        return m_arrSnds[n];
    }
}

CSnd *CSndArray::operator[](const char *name)
{
    for (int n = 0; n < m_nSize; ++n)
    {
        CSnd *snd = m_arrSnds[n];
        if (!strcmp(name, snd->getName()))
        {
            return snd;
        }
    }
    return nullptr;
}

bool CSndArray::hasSound(int i)
{
    return !(i < 0 || i >= m_nSize);
}

bool CSndArray::hasSound(const char *name)
{
    for (int i = 0; i < m_nSize; ++i)
    {
        if (!strcmp(name, m_arrSnds[i]->getName()))
        {
            return true;
        }
    }
    return false;
}

void CSndArray::debug()
{
    // for (int i=0; i< m_nSize; ++i) {
    // CSnd & snd = * m_arrSnds[i];
    // qDebug("snd %d = %s", i, snd.getName());
    //}
}

/////////////////////////////////////////////////////////////////////////////
// CSnd

CSnd::CSnd()
{
    init();
}

CSnd::CSnd(const CSnd &snd)
{
    init();
    copy(snd);
}

CSnd::CSnd(const CSnd *snd)
{
    init();
    copy(*snd);
}

CSnd::CSnd(const char *name, char *data, int size)
{
    init();
    m_name = name;
    m_data = data;
    m_size = size;
}

CSnd::~CSnd()
{
    if (m_data)
    {
        delete[] m_data;
        m_data = nullptr;
    }

    m_size = 0;
}

void CSnd::init()
{
    m_data = nullptr;
    m_size = 0;
    m_uid = rand();
}

/////////////////////////////////////////////////////////////////////////////
// CSnd message handlers

const char *CSnd::getName() const
{
    return m_name.c_str();
}

char *CSnd::getData() const
{
    return m_data;
}

int CSnd::getSize() const
{
    return m_size;
}

unsigned int CSnd::getUID() const
{
    return m_uid;
}

void CSnd::setName(const char *name)
{
    m_name = name;
}

void CSnd::replace(char *data, int size)
{
    if (m_data)
    {
        delete[] m_data;
    }

    m_data = data;
    m_size = size;
    m_uid = rand();
}

CSnd &CSnd::operator=(const CSnd &src)
{
    return copy(src);
}

CSnd &CSnd::copy(const CSnd &src)
{
    if (m_data)
    {
        delete[] m_data;
        m_data = nullptr;
    }

    m_size = src.getSize();
    if (m_size)
    {
        m_data = new char[m_size];
        memcpy(m_data, src.getData(), m_size);
    }

    m_name = src.getName();
    return *this;
}

bool CSnd::operator!=(const CSnd &src)
{
    if (m_size != src.getSize())
    {
        return true;
    }

    if ((!m_data || !src.getData()) && (m_data != src.getData()))
    {
        return true;
    }

    if (m_data && src.getData() && memcmp(m_data, src.getData(), m_size))
    {
        return true;
    }

    if (m_name != src.getName())
    {
        return true;
    }
    return false;
}

void CSnd::debug()
{
    if (m_data)
    {
        char t[5];
        t[4] = 0;
        memcpy(t, m_data, 4);
    }
}

bool CSnd::read(IFile &file)
{
    return true;
}

bool CSnd::write(IFile &file)
{
    if (getSize())
    {
        file.write(getData(), getSize());
    }
    return true;
}
