/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2014  Francois Blanchette

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

#ifndef _SND_H
#define _SND_H

#include <string>
#include "ISerial.h"

class CSnd: public ISerial
{
public:
    CSnd();
    CSnd(const char* name, char *data, const int size);
    CSnd(const CSnd & snd);
    CSnd(const CSnd * snd);
    ~CSnd();
    const char* getName() const;
    int getSize() const;
    char *getData() const;
    unsigned int getUID() const;
    void replace(char *data, int size);
    void setName (const char*);
    CSnd & copy(const CSnd & src);
    bool operator != (const CSnd & src);
    CSnd & operator = (const CSnd & src);
    void debug();
    virtual bool read(IFile & file);
    virtual bool write(IFile & file);

protected:
    void init();
    std::string m_name;
    char *m_data;
    int m_size;
    unsigned m_uid;
};

#endif
