#include "FileWrap.h"
#include "Display.h"
#include <cstdlib>
#include "stdafx.h"
#include "FileWrap.h"

/////////////////////////////////////////////////////////////////////
// CDisplay

CDisplay::CDisplay(const char* name, int x, int y, int type)
{
    m_uid = rand();
    m_name = name;
    m_x = x;
    m_y = y;
    setType(type);
    m_r = 0x20;
    m_g = 0x80;
    m_r = 0xf0;
    m_a = 0x80;
    m_visible = false;
    m_shadow = false;
    m_shadowX = 4;
    m_shadowY = 4;
    m_shadowR = 0;
    m_shadowG = 0;
    m_shadowB = 0;
    m_shadowA = 255;
    m_size = 24;
    m_imageSet = 0;
    m_imageNo = 0;
}

CDisplay::~CDisplay()
{
}

void CDisplay::setShadow( bool shadow, int tx, int ty )
{
    m_shadow = shadow;
    m_shadowX = tx;
    m_shadowY = ty;
}

void CDisplay::enableShadow( bool shadow )
{
    m_shadow = shadow;
}

void CDisplay::setShadowOffset( int tx, int ty )
{
    m_shadowX = tx;
    m_shadowY = ty;
}

void CDisplay::setShadowColor( int r, int g, int b, int a )
{
    m_shadowR = r;
    m_shadowG = g;
    m_shadowB = b;
    m_shadowA = a;
}

void CDisplay::setImage(int imageSet, int imageNo)
{
    m_type = DISPLAY_IMAGE;
    m_imageSet = imageSet;
    m_imageNo = imageNo;
}

void CDisplay::setAlpha(int alpha)
{
    m_a = alpha;
}

void CDisplay::setXY(int x, int y)
{
    m_x = x;
    m_y = y;
}

void CDisplay::setType(int type)
{
    m_type = type;
    switch (type) {
        case DISPLAY_TIME_LEFT:
            m_template = "%.3d";
        break;
        case DISPLAY_SCORE:
            m_template = "%.8d";
        break;
        case DISPLAY_LIVES:
            m_template = "%.2d";
        break;
        default:
            m_template = "";
    }
}

void CDisplay::setColor(int r, int g, int b, int a)
{
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
}

void CDisplay::setFontSize(int size)
{
    m_size = size;
}

void CDisplay::setExpireTime(int time)
{
    m_timeExpire = time;
}

void CDisplay::setVisible(bool visible)
{
    m_visible = visible;
}

void CDisplay::show()
{
    m_visible = true;
}

void CDisplay::hide()
{
    m_visible = false;
}

void CDisplay::setText(const char *content, int type)
{
    m_content = content;
    if (type != DISPLAY_SAME) {
        m_type = DISPLAY_MESSAGE;
    }
}

int CDisplay::x()
{
    return m_x;
}

int CDisplay::y()
{
    return m_y;
}

int CDisplay::type()
{
    return m_type;
}

bool CDisplay::visible()
{
    return m_visible;
}

const char* CDisplay::name() const
{
    return m_name.c_str();
}

void CDisplay::flip()
{
    m_visible = !m_visible;
}

int CDisplay::size()
{
    return m_size;
}

const char* CDisplay::text()
{
    return m_content.c_str();
}

int CDisplay::red()
{
    return m_r;
}

int CDisplay::green()
{
    return m_g;
}

int CDisplay::blue()
{
    return m_b;
}

int CDisplay::alpha()
{
    return m_a;
}

void CDisplay::write(IFile & file)
{
    unsigned int version = VERSION;
    file.write(&version, sizeof(version));
    file.write(&m_x, sizeof(int));
    file.write(&m_y, sizeof(int));
    file.write(&m_state, sizeof(int));
    file.write(&m_type, sizeof(int));
    file.write(&m_r, sizeof(int));
    file.write(&m_g, sizeof(int));
    file.write(&m_b, sizeof(int));
    file.write(&m_a, sizeof(int));
    file.write(&m_size, sizeof(int));
    file.write(&m_visible, sizeof(bool));
    file.write(&m_shadow, sizeof(bool));
    file.write(&m_shadowX, sizeof(int));
    file.write(&m_shadowY, sizeof(int));
    file.write(&m_timeExpire, sizeof(int));
    file.write(&m_shadowR, sizeof(int));
    file.write(&m_shadowG, sizeof(int));
    file.write(&m_shadowB, sizeof(int));
    file.write(&m_shadowA, sizeof(int));
    file.write(&m_imageSet, sizeof(int));
    file.write(&m_imageNo, sizeof(int));
    file.write(&m_protected, sizeof(bool));
    file << m_name;
    file << m_content;
    file << m_template;
}

void CDisplay::read(IFile &file, int version)
{
    unsigned int file_version = 0;
    file.read(&file_version, sizeof(file_version));
    ASSERT(file_version==VERSION);
    file.read(&m_x, sizeof(int));
    file.read(&m_y, sizeof(int));
    file.read(&m_state, sizeof(int));
    file.read(&m_type, sizeof(int));
    file.read(&m_r, sizeof(int));
    file.read(&m_g, sizeof(int));
    file.read(&m_b, sizeof(int));
    file.read(&m_a, sizeof(int));
    file.read(&m_size, sizeof(int));
    file.read(&m_visible, sizeof(bool));
    file.read(&m_shadow, sizeof(bool));
    file.read(&m_shadowX, sizeof(int));
    file.read(&m_shadowY, sizeof(int));
    file.read(&m_timeExpire, sizeof(int));
    file.read(&m_shadowR, sizeof(int));
    file.read(&m_shadowG, sizeof(int));
    file.read(&m_shadowB, sizeof(int));
    file.read(&m_shadowA, sizeof(int));
    file.read(&m_imageSet, sizeof(int));
    file.read(&m_imageNo, sizeof(int));
    file.read(&m_protected, sizeof(bool));
    file >> m_name;
    file >> m_content;
    file >> m_template;
}

const char * CDisplay::templateStr()
{
    return m_template.c_str();
}

void CDisplay::setTemplate(const char *s)
{
    m_template = s;
}

bool CDisplay::isProtected()
{
    return m_protected;
}

void CDisplay::setProtected(bool b)
{
    m_protected = b;
}

void CDisplay::setName(const char *name)
{
    m_name = name;
}

bool CDisplay::shadow()
{
    return m_shadow;
}

int CDisplay::shadowX()
{
    return m_shadowX;
}

int CDisplay::shadowY()
{
    return m_shadowY;
}

int CDisplay::shadowR()
{
    return m_shadowR;
}

int CDisplay::shadowG()
{
    return m_shadowG;
}

int CDisplay::shadowB()
{
    return m_shadowB;
}

int CDisplay::shadowA()
{
    return m_shadowA;
}

int CDisplay::imageSet()
{
    return m_imageSet;
}

int CDisplay::imageNo()
{
    return m_imageNo;
}
