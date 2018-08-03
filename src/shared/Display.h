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
#include <string>

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
    void setImage(int imageSet, int imageNo);
    void setAlpha(int alpha);
    void setXY(int x, int y);
    void setType(int type);
    void setColor(int r, int g, int b, int a);
    void setFontSize(int size);
    void setExpireTime(int time);
    void setVisible(bool visible);
    void setTemplate(const char *s);
    void setProtected(bool b);
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
    const char* name() const;
    const char* text();
    const char *templateStr();
    bool isProtected();
    int size();
    bool shadow();
    int imageSet();
    int imageNo();
    void flip();
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
    void write (IFile &file );
    void read ( IFile & file, int version );

protected:
    int m_x;
    int m_y;
    int m_state;
    int m_type;
    int m_r;
    int m_g;
    int m_b;
    int m_a;
    int m_size;
    bool m_visible;
    bool m_shadow;
    int m_shadowX;
    int m_shadowY;
    int m_timeExpire;
    int m_shadowR;
    int m_shadowG;
    int m_shadowB;
    int m_shadowA;
    int m_imageSet;
    int m_imageNo;

    int m_uid;
    std::string m_name;
    std::string m_content;
    std::string m_template;
    bool m_protected;
    enum {
        VERSION = 0x0001
    };
}; 

#endif
