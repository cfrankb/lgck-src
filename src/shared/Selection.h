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
#ifndef __CSELECTION__
#define __CSELECTION__
class CLevelEntry;

class CSelection
{
public:
    CSelection();
    ~CSelection();

    CLevelEntry & operator[] (int i);
    void operator = (CSelection & src);
    void operator += (CSelection & src);
    void addEntry(const CLevelEntry & entry, int index=-1);
    void clear();
    bool isSingle();
    bool isMulti();
    int getSize();
    int getIndex(int i);
    void setIndex(int i, int index);
    void removeFromIndex(int index);
    void removeAt(int i);
    void removeProto(int proto);
    void applyDelta(int dx, int dy);
    void forget();

protected:

    enum {
        GROWBY = 10
    };

    int m_size;
    int m_max;
    CLevelEntry *m_entries;
    int *m_index;
};
 
#endif
