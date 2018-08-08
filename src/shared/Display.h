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

#ifndef _DISPLAY_H
#define _DISPLAY_H
#include <unordered_map>
#include <string>
#include <list>

class IFile;

class CDisplay
{
public:
    ~CDisplay();
    CDisplay(const char * name = "", int x=0, int y=0, int type=0);
    void setName(const char *name);
    void setShadow( bool shadow, int tx, int ty );
    void enableShadow( bool shadow );
    void setShadowOffset(  int tx, int ty );
    void setShadowColor( int r, int g, int b, int a );
    void setImage(int imageSet, int imageNo, bool resetType=true);
    void setAlpha(int alpha);
    void setXY(int x, int y);
    void setType(int type, bool resetTemplate=true);
    void setColor(int r, int g, int b, int a);
    void setFontSize(int size);
    void setExpireTime(int time);
    void setVisible(bool visible);
    void setTemplate(const char *s);
    void setProtected(bool b);
    void setFlagXY(int flagX, int flagY);
    void show();
    void hide();
    void setText(const char* content, int displayType=DISPLAY_MESSAGE);
    int x();
    int y();
    int red();
    int green();
    int blue();
    int alpha();
    int type();
    int shadowX();
    int shadowY();
    int shadowR();
    int shadowG();
    int shadowB();
    int shadowA();
    bool visible();
    const char* name();
    const char* text();
    const char *templateStr();
    bool isProtected();
    int size();
    bool shadow();
    int imageSet();
    int imageNo();
    int flagX();
    int flagY();
    void flip();

    int geti(unsigned i);
    const char* gets(int i);
    void set(int i, int v);
    void set(int i, const char *v);
    std::list<std::string> & lines();

    enum {
        DISPLAY_TIME_LEFT       = 0,
        DISPLAY_MESSAGE         = 1,
        DISPLAY_SCORE           = 2,
        DISPLAY_LIVES           = 3,
        DISPLAY_HP              = 4,
        DISPLAY_DEBUGX          = 5,
        DISPLAY_IMAGE		    = 6,
        DISPLAY_SAME            = -1
    } DISPLAY_TYPE;    
    bool write(IFile &file );
    bool read(IFile & file);

    enum {
        FLAG_X_NONE,
        FLAG_X_ALIGN_LEFT,
        FLAG_X_ALIGN_RIGHT,
        FLAG_X_ALIGN_CENTER
    };

    enum {
        FLAG_Y_NONE,
        FLAG_Y_ALIGN_TOP,
        FLAG_Y_ALIGN_BOTTOM,
        FLAG_Y_ALIGN_CENTER
    };

private:

    std::unordered_map <int, std::string> m_attrs;
    std::unordered_map <int, int> m_attri;
    std::list<std::string> m_lines;
    void splitString(const char *inData);

    enum {
        // numbers
        DI_UUID,
        DI_X,
        DI_Y,
        DI_STATE,
        DI_TYPE,
        DI_R,
        DI_G,
        DI_B,
        DI_A,
        DI_SIZE,
        DI_VISIBLE,
        DI_SHADOW,
        DI_SHADOWX,
        DI_SHADOWY,
        DI_TIMEEXPIRE,
        DI_SHADOWR,
        DI_SHADOWG,
        DI_SHADOWB,
        DI_SHADOWA,
        DI_IMAGESET,
        DI_IMAGENO,
        DI_PROTECTED,
        DI_FLAG_X,
        DI_FLAG_Y
    };

    enum {
        // strings
        DI_NAME     = 50,
        DI_CONTENT  = 51,
        DI_TEMPLATE = 52
    };

    enum {
        VERSION = 0x0002
    };
}; 

#endif
