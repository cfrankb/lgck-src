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
#ifndef DISPLAYCONFIG_H
#define DISPLAYCONFIG_H

#include "ISerial.h"

class CDisplay;
class IFile;

class CDisplayConfig: public ISerial
{
public:
    CDisplayConfig();
    ~CDisplayConfig();

    int indexOf(const char *name);
    CDisplay *find(const char *name);
    void removeAt(int i);
    CDisplay & add(CDisplay &display);
    int getSize();
    void forget();
    CDisplay & add(const char *name, int x, int y, int type);
    void reset();
    virtual bool read(IFile & file);
    virtual bool write(IFile & file);
    void killFrameSet(int frameSet);
    void killFont(int fontID);
    CDisplay *operator[](int i);
    CDisplay *operator[](const char *s);

    enum {
        NOT_FOUND = -1
    };
protected:
    void resize();

    enum {
        GROWBY = 20,
        MAX = 32,
        VERSION = 2
    };

    int m_size;
    int m_max;
    CDisplay **m_displays;
};

#endif // DISPLAYCONFIG_H
