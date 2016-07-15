#include "Font.h"
#include "IFile.h"
#include <cstring>
#include "stdafx.h"

char CFont::m_signature[]="LGCKFONT";

CFont::CFont()
{
    m_pixels = NULL;
    m_text = "";
}

CFont::~CFont()
{
    forget();
}

void CFont::forget()
{
    if (m_pixels) {
        delete [] m_pixels;
        m_pixels= NULL;
    }
    m_width = 0;
    m_height = 0;
  //  m_text = "";
    m_glyphs.clear();
    m_props.clear();
}

void CFont::setPixmap(const char *glyphs, unsigned int *pixmap, int width, int height)
{
    if (m_pixels) {
        delete [] m_pixels;
        m_pixels= NULL;
    }
    m_pixels = new unsigned int[width*height];
    m_width = width;
    m_height = height;
    m_text = glyphs;
    memcpy(m_pixels, pixmap, width*height*sizeof(int));
}

void CFont::addGlyph(char id, CFont::Glyph & glyph)
{
    m_glyphs[id] = glyph;
}

CFont::Glyph & CFont::operator [](int i)
{
    return m_glyphs[i];
}

int CFont::getSize()
{
    return m_glyphs.size();
}

void CFont::write(IFile & file)
{
    // header
    int version = VERSION;
    file.write(m_signature, 8);
    file.write(&version, 4);
    file.write(&m_width, 2);
    file.write(&m_height, 2);
    file << m_text;
    file.write(&m_pxRef, 1);
    file.write(&m_scaleX, 1);
    file.write(&m_scaleY, 1);
    // glyphs
    int size = m_glyphs.size();
    file.write(&size, 2);
    for(auto kv : m_glyphs) {
        char key = kv.first;
        Glyph & glyph = kv.second;
        file.write(&key, 1);
        file.write(&glyph, sizeof(Glyph));
    }
    // props
    size = m_props.size();
    file.write(&size, 2);
    for(auto kv : m_props) {
        file << kv.first;
        file << kv.second;
    }
    // pixels
    file.write(m_pixels, m_width * m_height * sizeof(unsigned int));
}

void CFont::read(IFile & file)
{
    forget();
    char signature[8];
    int version;
    // header
    file.read(signature, 8);
    ASSERT(!memcmp(signature, m_signature,8));
    file.read(&version, 4);
    ASSERT(version==VERSION);
    file.read(&m_width, 2);
    file.read(&m_height, 2);
    file >> m_text;
    m_pxRef = m_scaleX = m_scaleY = 0;
    file.read(&m_pxRef, 1);
    file.read(&m_scaleX, 1);
    file.read(&m_scaleY, 1);
    // glyphs
    int size = 0;
    file.read(&size, 2);
    for (int i=0; i < size;++i) {
        Glyph glyph;
        char id;
        file.read(&id, 1);
        file.read(&glyph, sizeof(glyph));
        m_glyphs[id] = glyph;
    }
    // props
    file.read(&size, 2);
    for (int i=0; i < size;++i) {
        std::string key;
        std::string val;
        file >> key;
        file >> val;
        m_props[key]=val;
    }
    // pixels
    m_pixels = new unsigned int[m_width*m_height];
    file.read(m_pixels, m_width * m_height * sizeof(unsigned int));
    qDebug("w:%d h:%d total:%d",m_width, m_height, m_width * m_height * sizeof(unsigned int));
    for (int i=0; i < m_width*m_height; ++i) {
//        m_pixels[i]=0xff0000ff;
    }
}

void CFont::setScale(int px, int scaleX, int scaleY)
{
    m_pxRef = px;
    m_scaleX = scaleX;
    m_scaleY = scaleY;
}

int CFont::width()
{
    return m_width;
}

int CFont::height()
{
    return m_height;
}

unsigned int *CFont::pixels()
{
    return m_pixels;
}

void CFont::FaceSize(int size)
{
    ASSERT(m_pxRef);
    m_face = size;
    m_scale = (float)size / (float)m_pxRef * fixup();
   //qDebug("m_scale: %f %d/%d",m_scale, size, m_pxRef);
}

int CFont::Advance(const char *text)
{
    float width = 0;
    for (int i=0; text[i]; ++i) {
        char id = text[i];
        Glyph & glyph = m_glyphs[id];
        if (id == ' ') {
            glyph.width = scaleX() * 0.60;
        }
        width += glyph.width * m_scale;
    }
    return width;
}

float CFont::scale()
{
    return m_scale;
}

unsigned int CFont::textureId()
{
    return m_textureId;
}

void CFont::setTextureId(unsigned int textureId)
{
    m_textureId = textureId;
}

int CFont::scaleX()
{
    return m_scaleX;
}

int CFont::scaleY()
{
    return m_scaleY;
}

int CFont::face()
{
    return m_face;
}

float CFont::fixup()
{
    return 0.80;
}
