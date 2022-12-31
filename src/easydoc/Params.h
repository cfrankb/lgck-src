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
#ifndef PARAMS_H
#define PARAMS_H

#include <QString>
#include "FileWrap.h"

class Param
{
public:
    Param() {}
    ~Param() {}

    Param &operator=(Param &s)
    {
        name = s.name;
        type = s.type;
        desc = s.desc;
        flags = s.flags;
        return *this;
    }

    QString name;
    QString type;
    QString desc;
    int flags;
};

class CParams
{
public:
    CParams();
    ~CParams();

    Param &operator[](int i);
    const CParams &operator=(CParams &s);
    int add(Param &p);
    void removeAt(int i);
    int getSize();
    void forget();
    void read(CFileWrap &file, int version);
    void write(CFileWrap &file);
    void debug();

protected:
    enum
    {
        MAX_PARAMS = 32
    };
    int m_size;
    int m_max;
    Param *m_params;
};

#endif // PARAMS_H
