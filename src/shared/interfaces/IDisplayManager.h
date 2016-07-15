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

#ifndef IDISPLAYMANAGER_H
#define IDISPLAYMANAGER_H

class CDisplay;

class IDisplayManager
{
public:
    virtual ~IDisplayManager(){}
    virtual CDisplay & add (const char* name, int x, int y, int type)=0;
    virtual CDisplay & add (CDisplay & display)=0;
    virtual void draw()=0;
    virtual void drawText(CDisplay & display)=0;
    virtual void drawLives(CDisplay & display)=0;
    virtual void drawImage(CDisplay & display)=0;
    virtual void drawText(int x, int y, const char *text, int fontID=0, int fontSize=10, unsigned int rgba=0xffffff, int shadowOffset=0, unsigned int shadowColor=0)=0;
    virtual CDisplay & operator [] (int i)=0;
    virtual CDisplay & operator [] (const char* s)=0;
    virtual CDisplay & getAt(int i)=0;
    virtual int findDisplay(const char * name)=0;
    virtual bool isValidIndex(int i)=0;
    virtual void removeAll()=0;
    virtual void removeAt(int i)=0;
    virtual int getSize()=0;
    virtual const char *signature()=0;
    virtual int display_sizeText(int displayId, const char *text=NULL)=0;
    friend class CDisplay;
protected:
    enum {
        GROWBY = 20,
        VERSION = 0x0
    };
    CDisplay *m_displays;
    int m_size;
    int m_max;
};

#endif
