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
#ifndef _PROTO_INDEX_H
#define _PROTO_INDEX_H

#include <string>
class CProtoArray;

class CProtoIndex
{

public:

    enum {
        FILTER_NONE = 0,
        FILTER_BACKGROUND = 1,
        FILTER_OBJECTS = 2,
        FILTER_PLAYER = 3,
        FILTER_MONSTER = 4,
        FILTER_BULLET = 5,
    };

    CProtoIndex(CProtoArray *parent, int custom = FILTER_NONE);
    ~CProtoIndex();

    void init();
    int findPos (int protoId);
    int findProto (int protoId);

    //int getAtIndex( int i );
    void removeIndex (int pos );
    void removeFromIndex (int protoId);
    int insert(int protoId);
    void resizeIndex(int newSize);
    int getSize();
    void forget();
    void debug();
    int operator [] (int i);
    static const char* getFilterName(int i);
    static int getFilterCount();
    void setTextFilter(const char *search);

protected:

    bool isAccepted(int protoId);
    bool matchString(int protoId);

    std::string m_search;
    int *m_index;
    int m_size;
    int m_custom;
    CProtoArray * m_protoArray;

    static std::string m_arrFilters[];
};
#endif
