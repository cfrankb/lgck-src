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
#ifndef FONT_H
#define FONT_H

#include <unordered_map>
#include <string>
class IFile;

class CFont
{
public:
    CFont();
    ~CFont();
    bool write(IFile & file);
    bool read(IFile & file);

    typedef struct
    {
        short x;
        short y;
        short width;
        short height;
    } Glyph;

    void FaceSize(int size);
    int Advance(const char *text);
    void setPixmap(const char *glyphs, unsigned int *pixmap, int width, int height);
    void setScale(int px, int scaleX, int scaleY);
    void addGlyph(char id, Glyph & glyph);
    void forget();
    Glyph & operator [](int i);
    int getSize();
    int width();
    int height();
    float scale();
    unsigned int *pixels();
    unsigned int textureId();
    void setTextureId(unsigned int textureId);
    int scaleX();
    int scaleY();
    int face();
    float fixup();
    CFont &operator = (CFont & s);

protected:
    enum {
        VERSION_1 = 0x0001,
        VERSION_2 = 0x0002
    };

    inline int pow2roundup(int x)
    {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x+1;
    }

    std::unordered_map<char, CFont::Glyph> m_glyphs;
    std::unordered_map<std::string, std::string> m_props;
    std::string m_text;
    unsigned int *m_pixels;
    int m_width;
    int m_height;
    int m_pxRef;
    int m_scaleX;
    int m_scaleY;
    float m_scale;
    int m_face;
    unsigned int m_textureId;
};

#endif // FONT_H
