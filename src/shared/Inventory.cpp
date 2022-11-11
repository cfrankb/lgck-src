/*
    LGCK Builder Runtime
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

#include "Inventory.h"
#include "IFile.h"
#include <cstring>

CInventory::CInventory()
{
    m_options = 0;
    m_name[0] = 0;
    reset();
}

void CInventory::addItem(int protoId, int count)
{
    for (int i=0; i < MAX_ITEMS; ++i) {
        if (!m_items [ i ].itemProto
            || m_items [ i ].itemProto == protoId) {
            m_items [ i ].count += count;
            m_items [ i ].itemProto = protoId;
            break;
        }
    }
}

bool CInventory::hasItem( int protoId )
{
    for (int i = 0; i < MAX_ITEMS; ++i) {
        if (m_items [ i ].itemProto == protoId)
            return true;
    }
    return false;
}

void CInventory::reset()
{
    memset(&m_items, 0, sizeof(m_items));
    m_options=0;
    m_name[0]=0;
}

void CInventory::removeItem(int protoId)
{
    for (int i=0; i < MAX_ITEMS; ++i) {
        if (m_items [ i ].itemProto == protoId) {
            if (! (-- m_items [ i ].count) ) {
                m_items [ i ].itemProto = 0;
            }
            break;
        }
    }
}

int CInventory::getSize() const
{
   return MAX_ITEMS;
}

int CInventory::operator[] (int i) const
{
    return m_items[i].itemProto;
}

void CInventory::setName(const char *s)
{
    m_name = s;
}

const char* CInventory::getName() const
{
    return m_name.c_str();
}

void CInventory::setOptions(int options)
{
    m_options = options;
}

int CInventory::getOptions() const
{
    return m_options;
}

void CInventory::operator =(const CInventory &s)
{
    setName(s.getName());
    setOptions(s.getOptions());
    for (int i=0; i < s.getSize(); ++i) {
        m_items[i] = s.m_items[i];
    }
}

void CInventory::read(IFile & file)
{
    reset();
    std::string name;
    file >> m_name;
    file.read(&m_options, sizeof(m_options));
    int size = 0;
    file.read(&size, sizeof(size));
    for (int i=0; i < size; ++i) {
        file.read(&m_items[i],sizeof(ITEM));
    }
}

void CInventory::write(IFile &file)
{
    file << std::string(m_name);
    file.write(&m_options, sizeof(m_options));
    int size = 0;
    for (int i=0; i < getSize(); ++i) {
        if (m_items[i].itemProto) {
            ++size;
        }
    }
    file.write(&size, sizeof(size));
    for (int i=0; i < getSize(); ++i) {
        if (m_items[i].itemProto) {
            file.write(&m_items[i],sizeof(ITEM));
        }
    }
}
