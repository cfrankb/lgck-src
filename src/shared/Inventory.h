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

#ifndef INVENTORY_H
#define INVENTORY_H

#include <inttypes.h>
class IFile;

class CInventory
{

public:

    void addItem(int protoId, int count=1);
    void reset();
    bool hasItem( int nProtoId );
    void removeItem(int nProtoId);
    int getSize() const;
    void setName(const char *s);
    const char* getName() const;
    int operator[] (int i) const;
    void setOptions(int options);
    int getOptions() const;
    void operator = (const CInventory& s);
    void read(IFile &file);
    void write(IFile & file);

    CInventory();

protected:
    enum {
        MAX_ITEMS          = 16,
        MAX_NAME           = 32
    };
    typedef struct {
        uint16_t itemProto;
        uint16_t count;
    } ITEM;

    char m_name [ MAX_NAME ];
    ITEM m_items [ MAX_ITEMS ];
    int m_options;
};

#endif // INVENTORY_H
