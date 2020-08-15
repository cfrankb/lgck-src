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
#ifndef FONTMANAGER_H
#define FONTMANAGER_H

class IFile;
class CFont;
#include <string>
#include "ISerial.h"

#define DEFAULT_FONT "Arial"

class CFontManager: public ISerial
{
public:
    CFontManager();
    ~CFontManager();

    int indexOf(const char *name);
    CFont *find(const char *name);
    const char* nameAt(int i);
    void removeAt(int i);
    CFont & add(CFont & font, const char * name);
    int getSize();
    void forget();
    void reset();
    virtual bool read(IFile & file);
    virtual bool write(IFile & file);
    CFont *operator[](int i);
    CFont *operator[](const char *s);
    CFont *at(int i);
    void setName(int i, const char *name);

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

    typedef struct {
        CFont * font;
        std::string name;
    } FONT;

    void deleteFont(FONT *font);

    int m_size;
    int m_max;
    FONT **m_fonts;
};

#endif // FONTMANAGER_H
