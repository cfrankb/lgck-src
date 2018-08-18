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

#ifndef GR_OPENGL_H
#define GR_OPENGL_H

#include "../shared/interfaces/IGraphics.h"

class CFrame;
class CIMOpengl;
class CGame;
class IDisplayManager;
class CDisplayManager;
class CDMOpengl;
class CFont;
class ISurface;

#ifdef LGCK_QT
    #include <QOpenGLFunctions_2_0>
#endif

class CGROpenGL : public IGraphics
#ifdef LGCK_QT
    , protected QOpenGLFunctions_2_0
#endif
{
public:
    CGROpenGL(CGame *game);
    virtual ~CGROpenGL();
    virtual const char* signature();
    virtual void getScreenSize(int & len, int & hei);
    virtual void clear(unsigned int red, unsigned int green, unsigned int blue);
    virtual void clear(unsigned int rgb);
    virtual void paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill=true);
    virtual void paintImage(int x1, int y1, int frameSet, int frameNo);
    virtual void render(CFont & font, const char *text, int x, int y, const Color & color);
    virtual IDisplayManager* displayManager();
    virtual void ss_paint(int x1, int y1, int x2, int y2, unsigned int rgba, bool fill=true);
    virtual void ss_paintImage(int x1, int y1, int frameSet, int frameNo);
    virtual IImageManager *cache();

protected:
    CIMOpengl *m_imageManager;
    std::unordered_map<std::string, unsigned int> m_fonts;
    void drawSurface(ISurface * surface, int mx, int my);
    inline int pow2roundup (int x) {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x+1;
    }
    enum {
        ALL_ALPHA = 0xff000000
    };
};

#endif // GR_OPENGL_H
