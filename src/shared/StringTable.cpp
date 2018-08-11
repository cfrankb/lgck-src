/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2013  Francois Blanchette

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
#include "StringTable.h"
#include "IFile.h"
#include "LevelEntry.h"

CStringTable::CStringTable()
{
    m_size = 1;
    m_max = m_size + GROW_BY;
    m_strings = new std::string[m_max];
}

CStringTable::~CStringTable()
{
    if (m_strings) {
        delete [] m_strings;
    }
    m_strings = NULL;
}

bool CStringTable::read(IFile &file)
{
    file.read(&m_size, sizeof(UINT32));
    if (m_strings) {
        delete [] m_strings;
    }
    m_max = m_size + GROW_BY;
    m_strings = new std::string[m_max];
    for (int i=0; i<m_size; ++i) {
        file >> m_strings[i];
    }
    return true;
}

bool CStringTable::write(IFile &file)
{
    file.write(&m_size, sizeof(UINT32));
    for (int i=0; i<m_size; ++i) {
        file << m_strings[i];
    }
    return true;
}

const char * CStringTable::operator [] (int i)
{
    if (i < 0 || i >= m_size) {
        return m_strings[0].c_str();
    } else {
        return m_strings[i].c_str();
    }
}

int CStringTable::add(const char * str)
{
    if (str[0]) {
        int j = findFree();
        if (j == -1) {
            if (m_size==m_max) {
                m_max += GROW_BY;
                std::string *tmp = new std::string[m_max];
                for (int i=0; i<m_size; ++i) {
                    tmp[i] = m_strings[i];
                }
                delete [] m_strings;
                m_strings = tmp;
            }
            m_strings[m_size] = str;
            return m_size ++;
        } else {
            m_strings[j] = str;
            return j;
        }
    } else {
        return 0;
    }
}

void CStringTable::forget()
{
    m_size = 1;
}

int CStringTable::set(int i, const char *str)
{
    m_strings[i] = str;
    if (!str[0]) {
        return 0;
    } else {
        return i;
    }
}

int CStringTable::findFree()
{
    for (int i=1; i < m_size; ++i) {
        if (m_strings[i].empty()) {
            return i;
        }
    }
    return -1;
}

int CStringTable::duplicate(int i)
{
    return add(m_strings[i].c_str());
}

const char *CStringTable::get(int i)
{
    if (i < 0 || i >= m_size) {
        return m_strings[0].c_str();
    } else {
        return m_strings[i].c_str();
    }
}
