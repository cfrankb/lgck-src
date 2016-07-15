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

#ifndef IMUSIC_H
#define IMUSIC_H

class IMusic
{
public:
    virtual ~IMusic()=0;
    virtual bool open(const char *file)=0;
    virtual bool play(int loop=-1)=0;
    virtual void stop()=0;
    virtual void close()=0;
    virtual bool isValid()=0;
    virtual const char *signature() const=0;

protected:
    bool m_valid;
};

inline IMusic::~IMusic(){}

#endif // IMUSIC_H
