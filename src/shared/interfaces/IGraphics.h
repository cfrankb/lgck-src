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

#ifndef IGRAPHICS_H
#define IGRAPHICS_H

class CFrame;
class IDisplayManager;
class IImageManager;
class CFont;
class CGame;
class ISurface;
class CDisplayManager;

#include "Color.h"

class IGraphics
{
public:
    virtual ~IGraphics()=0;
    virtual const char* signature()=0;
    virtual void getScreenSize(int & len, int & hei)=0;
    virtual void clear(unsigned int red, unsigned int green, unsigned int blue)=0;
    virtual void clear(unsigned int rgb)=0;
    virtual void paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill=true)=0;
    virtual void paintImage(int x1, int y1, int frameSet, int frameNo)=0;
    virtual void render(CFont & font, const char *text, int x, int y, const Color & color)=0;
    virtual IDisplayManager* displayManager()=0;
    virtual void ss_paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill=true)=0;
    virtual void ss_paintImage(int x1, int y1, int frameSet, int frameNo)=0;
    virtual IImageManager *cache()=0;
    virtual bool isFlipped()=0;

    void getOffset(int & offsetX, int & offsetY);
    void drawScreen();

protected:
    CDisplayManager *m_displayManager;
    CGame *m_game;
    void drawSurface(ISurface * surface, int mx, int my);

    struct {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    } m_colorMod;
};

inline IGraphics::~IGraphics(){}

#endif // IGRAPHICS_H
