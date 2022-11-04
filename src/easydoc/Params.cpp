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
#include "Params.h"
#include "stdafx.h"
#include "../shared/qtgui/cheat.h"

CParams::CParams()
{
    m_size = 0;
    m_max = MAX_PARAMS;
    m_params = new Param[m_max];
}

CParams::~CParams()
{
    if (m_params) {
        delete [] m_params;
    }
}

Param & CParams::operator [] (int i)
{
    return m_params[i];
}

int CParams::add(Param & p)
{
    if (m_size == m_max) {
        m_max += MAX_PARAMS;
        Param *t = new Param[m_max];
        for (int i=0; i < m_size; ++i) {
            t[i] = m_params[i];
        }
        delete [] m_params;
        m_params = t;
    }

    m_params [ m_size ] = p;
    return m_size++;
}

void CParams::removeAt(int i)
{
    for (int j=i; j < m_size - 1; ++j) {
        m_params [ j ] = m_params [ j + 1 ];
    }
    --m_size;
}

int CParams::getSize()
{
    return m_size;
}

void CParams::forget()
{
    m_size = 0;
}

void CParams::read(CFileWrap & file, int version)
{
    m_size = 0;
    int size;
    file >> size;
    for (int i = 0; i < size; ++i) {
        Param p;
        file >> p.name;
        file >> p.type;
        file >> p.desc;
        if (version > 1) {
            file >> p.flags;
        } else {
            p.flags = 0;
        }
        add(p);
    }
}

void CParams::write(CFileWrap & file)
{
    file << m_size;
    for (int i = 0; i < m_size; ++i) {
        Param & p = m_params[i];
        file << p.name.trimmed();
        file << p.type.trimmed();
        file << p.desc.trimmed();
        file << p.flags;
    }
}

void CParams::debug()
{
    qDebug("\n");
    for (int i=0; i < m_size; ++i) {
        qDebug("%d> %s %s",
               i,
               q2c(m_params[i].type),
               q2c(m_params[i].name));
    }
    qDebug("\n");
}

const CParams & CParams::operator = (CParams & s)
{
    forget();
    for (int i=0; i<s.getSize(); ++i) {
        add(s[i]);
    }

    return *this;
}
