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
#ifndef IM_SFML_H
#define IM_SFML_H

#include "../shared/interfaces/IDisplayManager.h"
#include "Display.h"
#include "Color.h"

class CGame;
class CIMSfml;
class CGRSfml;
class CFont;
class IImageManager;
class IGraphics;

class CDisplayManager: public IDisplayManager
{
public:
    CDisplayManager(CGame *game, IImageManager *im, IGraphics * gm);
public:
    virtual ~CDisplayManager();
    virtual CDisplay & add (const char* name, int x, int y, int type);
    virtual CDisplay & add (CDisplay & display);
    virtual void draw();
    virtual void drawText(CDisplay & display);
    virtual void drawLives(CDisplay & display);
    virtual void drawCounter(CDisplay & display);
    virtual void drawImage(CDisplay & display);
    virtual void drawText(int x, int y, const char *text, int fontID=0, int fontSize=10, unsigned int rgba=0xffffff, int shadowOffset=0, unsigned int shadowColor=0);
    virtual CDisplay & operator [] (int i);
    virtual CDisplay & operator [] (const char* s);
    virtual CDisplay & get(const char* s);
    virtual CDisplay & getAt(int i);
    virtual int findDisplay(const char * name);
    virtual bool isValidIndex(int i);
    virtual void removeAll();
    virtual void removeAt(int i);
    virtual int getSize();
    virtual const char *signature();
    virtual int display_sizeText(int displayId, const char *text=NULL);
    virtual int indexOf(const char *name);
    virtual int computeY(CDisplay & display, int sy);
    virtual int computeX(CDisplay & display, int sx);
    virtual void drawHP();
    virtual void drawInventory();
    void drawDisplay(CDisplay & display);

protected:
    CGame *m_game;
    IImageManager *m_imageManager;
    IGraphics *m_graphics;

};

#endif
