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
#include "inventoryTable.h"
#include "Inventory.h"
#include "IFile.h"
#include <string.h>

CInventoryTable::CInventoryTable()
{
    m_max = GROW_BY;
    m_size = 1;
    m_inventories = new CInventory[m_max];
}

CInventoryTable::~CInventoryTable()
{
    if (m_inventories) {
        delete [] m_inventories;
    }
}

CInventory * CInventoryTable::operator[] (int i)
{
    return & m_inventories[i];
}

CInventory * CInventoryTable::operator[] (const char *s)
{
    for (int i=1; i < m_size; ++i) {
        if (!strcmp(m_inventories[i].getName(), s)){
            return & m_inventories[i];
        }
    }
    return nullptr;
}

void CInventoryTable::reset(bool hard)
{
    for (int i=1; i < m_size; ++i) {
        CInventory & inv = m_inventories[i];
        if (inv.getOptions() != IT_PERSISTENT || hard) {
            inv.reset();
            inv.setOptions(IT_FREE);
        }
    }
}

int CInventoryTable::indexOf(const char *name) const
{
    for (int i = 1; i < m_size; ++i) {
        CInventory & inv = m_inventories[i];
        if (!strcmp(inv.getName(), name)){
            return i;
        }
    }
    return -1;
}

int CInventoryTable::addInventory(const char *name, int options)
{
    int i = indexOf(name);
    if (i != -1) {
        CInventory & inv = m_inventories[i];
        inv.setOptions(options);
        return i;
    }
    i = findFree();
    if (i == -1) {
        if (m_size == m_max){
            m_max += GROW_BY;
            CInventory *t = new CInventory[m_max];
            for (i = 0; i < m_size; ++i) {
                t[i] = m_inventories[i];
            }
            delete [] m_inventories;
            m_inventories = t;
        }
        i = m_size++;
    }
    CInventory & inv = m_inventories[i];
    inv.reset();
    inv.setName(name);
    inv.setOptions(options);
    return i;
}

int CInventoryTable::findFree()
{
    for (int i=1; i < m_size; ++i) {
        CInventory & inv = m_inventories[i];
        if (inv.getOptions() == IT_FREE) {
            return i;
        }
    }
    return -1;
}

void CInventoryTable::write(IFile &file)
{
    file.write(&m_size, sizeof(m_size));
    for (int i = 1; i < m_size; ++i) {
        CInventory & inv = m_inventories[i];
        inv.write(file);
    }
}

void CInventoryTable::read(IFile &file)
{
    // reset hard
    reset(true);

    // Read inventories
    int size;
    file.read(&size, sizeof(size));
    for (int i = 1; i < size; ++i) {
        addInventory("", 0);
        CInventory & inv = m_inventories[i];
        inv.read(file);
    }
}
