/*
    LGCK Builder GUI
    Copyright (C) 1999, 2012  Francois Blanchette

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

#include "PathBlock.h"
#include "Path.h"
#include "IFile.h"

CPathBlock::CPathBlock()
{
    m_size = 1;
    m_max = GROW_BY;
    m_paths = new CPath[m_max];
}

CPathBlock::~CPathBlock()
{
    delete [] m_paths;
}

bool CPathBlock::read(IFile & file)
{
    uint32_t version = 0;
    file.read(&version, sizeof(uint32_t));
    file.read(&m_size, sizeof(uint32_t));
    delete [] m_paths;
    m_max = m_size + GROW_BY;
    m_paths = new CPath[m_max];
    for (int i=0; i<m_size; ++i) {
        m_paths[i].read(file);
    }
    return true;
}

bool CPathBlock::write(IFile &file)
{
    uint32_t version = VERSION;
    file.write(&version, sizeof(uint32_t));
    file.write(&m_size, sizeof(uint32_t));
    for (int i=0; i<m_size; ++i) {
        m_paths[i].write(file);
    }
    return true;
}

void CPathBlock::forget()
{
    m_size = 1;
}

CPath & CPathBlock::operator [](int i)
{
    return m_paths[i];
}

int CPathBlock::addPath()
{
    int id = findFree();
    if (id == -1) {
        if (m_size == m_max){
            m_max += GROW_BY;
            CPath *t = new CPath[m_max];
            for (int i=0; i < m_size; ++i) {
                t[i] = m_paths[i];
            }
            delete [] m_paths;
            m_paths = t;
        }
        return m_size++;
    } else {
        return id;
    }
}

void CPathBlock::resetPath(int id)
{
    m_paths[id].forget();
}

int CPathBlock::findFree()
{
    for (int i=1; i < m_size; ++i) {
        if (!m_paths[i].getSize()) {
            return i;
        }
    }
    return -1;
}

int CPathBlock::copyPath(int id)
{
    int newId = addPath();
    m_paths[newId] = m_paths[id];
    return newId;
}

CPath & CPathBlock::getPath(int id)
{
    return m_paths[id];
}

int CPathBlock::duplicate(int id)
{
    return (id ? copyPath(id) : id);
}
