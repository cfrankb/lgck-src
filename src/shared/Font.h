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
    void write(IFile & file);
    void read(IFile & file);

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
        VERSION = 0x0001
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
