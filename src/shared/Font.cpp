#include "Font.h"
#include "IFile.h"
#include <cstring>
#include "stdafx.h"
#include "helper.h"
#include <zlib.h>

#define SIGNATURE "LGCKFONT"

CFont::CFont()
{
    m_pixels = NULL;
    m_text = "";
    m_textureId = 0;
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
    m_textureId = 0;
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

bool CFont::write(IFile & file)
{
    // header
    int version = VERSION_2;
    const char *signature = SIGNATURE;
    file.write(signature, 8);
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
        file.write(&glyph.x, 2);
        file.write(&glyph.y, 2);
        file.write(&glyph.width, 2);
        file.write(&glyph.height, 2);
    }
    // props
    size = m_props.size();
    file.write(&size, 2);
    for(auto kv : m_props) {
        file << kv.first;
        file << kv.second;
    }
    // pixels
    unsigned char *out_data;
    unsigned long out_size;
    compressData(reinterpret_cast<unsigned char*>(m_pixels), m_width * m_height * sizeof(unsigned int), &out_data, out_size);
    file.write(&out_size, 4);
    file.write(out_data, out_size);
    delete [] out_data;
    return true;
}

bool CFont::read(IFile & file)
{
    forget();
    char signature[8];
    int version = 0;
    // header
    file.read(signature, 8);
    ASSERT(!memcmp(signature, SIGNATURE,8));
    file.read(&version, 4);
    //ASSERT(version==VERSION);
    if (version != VERSION_1 && version != VERSION_2) {
        qDebug("invalid version: %d", version);
        return false;
    }
    m_width = m_height = 0;
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
        memset(&glyph, 0, sizeof(glyph));
        file.read(&glyph.x, 2);
        file.read(&glyph.y, 2);
        file.read(&glyph.width, 2);
        file.read(&glyph.height, 2);
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
    int h = pow2roundup(m_height);
    m_height = h;
    m_pixels = new unsigned int[m_width*h];
    unsigned long totalSize = m_width * m_height * sizeof(unsigned int);
    if (version == VERSION_1) {
        file.read(m_pixels, totalSize);
    } else if (version == VERSION_2) {
        unsigned long tmpSize = 0;
        file.read(&tmpSize, 4);
        unsigned char *tmp = new unsigned char[tmpSize];
        file.read(tmp, tmpSize);
        int err = uncompress(reinterpret_cast<unsigned char*>(m_pixels),
                             &totalSize,
                             tmp,
                             tmpSize);
        if (err) {
            qDebug("CFont::Read err=%d\n", err);
            return false;
        }
    }
    return true;
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

CFont &CFont::operator = (CFont & s)
{
    m_glyphs = s.m_glyphs;
    m_props = s.m_props;
    m_text = s.m_text;
    m_width = s.m_width;
    m_height = s.m_height;
    m_pxRef = s.m_pxRef;
    m_scaleX = s.m_scaleX;
    m_scaleY = s.m_scaleY;
    m_scale = s.m_scale;
    m_face = s.m_face;
    m_textureId = s.m_textureId;
    int h = pow2roundup(m_height);
    m_pixels = new unsigned int[m_width*h];
    if (s.pixels()) {
        memcpy(m_pixels, s.pixels(), m_width * m_height * 4);
    } else {
        memset(m_pixels, 0, m_width * m_height * 4);
    }
    return * this;
}

