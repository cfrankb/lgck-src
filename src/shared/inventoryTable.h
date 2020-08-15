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
#ifndef INVENTORYTABLE_H
#define INVENTORYTABLE_H

class CInventory;
class IFile;

class CInventoryTable
{
public:
    CInventoryTable();
    ~CInventoryTable();
    CInventory * operator[] (int i);
    CInventory * operator[] (const char *s);
    enum {
        IT_DISCARDABLE,
        IT_PERSISTENT,
        IT_FREE
    };
    void reset(bool hard=false);
    int addInventory(const char *name, int options=IT_DISCARDABLE);
    int indexOf(const char *name) const;
    void read(IFile & file);
    void write(IFile & file);
protected:
    enum {
        GROW_BY = 10
    };
    int m_size;
    int m_max;
    CInventory *m_inventories;
    int findFree();
};

#endif // INVENTORYTABLE_H
