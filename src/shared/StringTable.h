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

#ifndef STRINGTABLE_H
#define STRINGTABLE_H

class IFile;

#include <string>

class CStringTable
{
public:
    CStringTable();
    ~CStringTable();

    void read(IFile & file);
    void write(IFile & file);
    const char * operator [] (int i);
    int add(const char *str);
    void forget();
    int set(int i, const char *str);
    const char * get(int i);
    int findFree();
    int duplicate(int i);

protected:
    enum {
        GROW_BY = 20
    };

    int m_size;
    int m_max;
    std::string * m_strings;
};

#endif // STRINGTABLE_H
