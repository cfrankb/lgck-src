#include "FileWrap.h"
#include "Display.h"
#include <cstdlib>
#include "stdafx.h"
#include "FileWrap.h"

/////////////////////////////////////////////////////////////////////
// CDisplay

CDisplay::CDisplay(const char* name, int x, int y, int type)
{
    set(DI_NAME, name);
    set(DI_X, x);
    set(DI_Y, y);
    setType(type);
    set(DI_R, 0xff);
    set(DI_G, 0xff);
    set(DI_B, 0xff);
    set(DI_A, 0xff);
    set(DI_VISIBLE, 0);
    set(DI_SHADOW, 0);
    set(DI_SHADOWX, 4);
    set(DI_SHADOWY, 4);
    set(DI_SHADOWR, 0);
    set(DI_SHADOWG, 0);
    set(DI_SHADOWB, 0);
    set(DI_SHADOWA, 0xff);
    set(DI_SIZE, 24);
    set(DI_IMAGESET,0);
    set(DI_IMAGENO,0);
}

CDisplay::~CDisplay()
{
}

void CDisplay::setShadow( bool shadow, int tx, int ty )
{
    set(DI_SHADOW, shadow ? 1 : 0);
    set(DI_SHADOWX, tx);
    set(DI_SHADOWY, ty);
}

void CDisplay::enableShadow( bool shadow )
{
    set(DI_SHADOW, shadow ? 1 : 0);
}

void CDisplay::setShadowOffset( int tx, int ty )
{
    set(DI_SHADOWX, tx);
    set(DI_SHADOWY, ty);
}

void CDisplay::setShadowColor( int r, int g, int b, int a )
{
    set(DI_SHADOWR, r);
    set(DI_SHADOWG, g);
    set(DI_SHADOWB, b);
    set(DI_SHADOWA, a);
}

void CDisplay::setImage(int imageSet, int imageNo, bool resetType)
{
    if (resetType) {
        set(DI_TYPE, DISPLAY_IMAGE);
    }
    set(DI_IMAGESET, imageSet);
    set(DI_IMAGENO, imageNo);
}

void CDisplay::setAlpha(int alpha)
{
     set(DI_SHADOWA, alpha);
}

void CDisplay::setXY(int x, int y)
{
    set(DI_X, x);
    set(DI_Y, y);
}

void CDisplay::setType(int type, bool resetTemplate)
{
    set(DI_TYPE, type);
    if (resetTemplate) {
        switch (type) {
            case DISPLAY_TIME_LEFT:
                set(DI_TEMPLATE, "%.3d");
            break;
            case DISPLAY_SCORE:
                set(DI_TEMPLATE, "%.8d");
            break;
            case DISPLAY_LIVES:
                set(DI_TEMPLATE, "%.2d");
            break;
            default:
                set(DI_TEMPLATE, "");
        }
    }
}

void CDisplay::setColor(int r, int g, int b, int a)
{
    set(DI_R, r);
    set(DI_G, g);
    set(DI_B, b);
    set(DI_A, a);
}

void CDisplay::setFontSize(int size)
{
   set(DI_SIZE, size);
}

void CDisplay::setExpireTime(int time)
{
    set(DI_TIMEEXPIRE, time);
}

void CDisplay::setVisible(bool visible)
{
    set(DI_VISIBLE, visible ? 1 : 0);
}

void CDisplay::show()
{
    set(DI_VISIBLE, true);
}

void CDisplay::hide()
{
    set(DI_VISIBLE, false);
}

void CDisplay::setText(const char *content, int type)
{
    set(DI_CONTENT, content);
    if (type != DISPLAY_SAME) {
        set(DI_TYPE, DISPLAY_MESSAGE);
    }
}

int CDisplay::x()
{
    return geti(DI_X);
}

int CDisplay::y()
{
    return geti(DI_Y);
}

int CDisplay::type()
{
    return geti(DI_TYPE);
}

bool CDisplay::visible()
{
    return geti(DI_VISIBLE) ? true : false;
}

const char* CDisplay::name()
{
    return gets(DI_NAME);
}

void CDisplay::flip()
{
    set(DI_VISIBLE, !geti(DI_VISIBLE) ? 1 : 0);
}

int CDisplay::size()
{
    return geti(DI_SIZE);
}

const char* CDisplay::text()
{
    return gets(DI_CONTENT);
}

int CDisplay::red()
{
    return geti(DI_R);
}

int CDisplay::green()
{
    return geti(DI_G);
}

int CDisplay::blue()
{
    return geti(DI_B);
}

int CDisplay::alpha()
{
    return geti(DI_A);
}

bool CDisplay::write(IFile & file)
{
    unsigned int version = VERSION;
    file.write(&version, sizeof(version));

    int intList[] =
    {
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

    int stringList [] = {
        // strings
        DI_NAME    ,
        DI_CONTENT  ,
        DI_TEMPLATE
    };

    unsigned int count = sizeof(intList) / sizeof(int);
    unsigned int realCount = 0;
    for (unsigned int i=0; i < count; ++i) {
        int k = intList[i];
        if (m_attri[k]) {
            ++realCount;
        }
    }

    file.write(&realCount, 1);
    for (unsigned int i=0; i < count; ++i) {
        int k = intList[i];
        int v = m_attri[k];
        if (v) {
            file.write(&k, 1);
            file.write(&v, 4);
        }
    }

    count = sizeof(stringList) / sizeof(int);
    file.write(&count, 1);
    for (unsigned int i=0; i < count; ++i) {
        int k = stringList[i];
        file.write(&k, 1);
        file << std::string(m_attrs[k]);
    }
    return true;
}

bool CDisplay::read(IFile &file)
{
    m_attrs.clear();
    m_attri.clear();
    unsigned int file_version = 0;
    file.read(&file_version, sizeof(file_version));
    if (file_version != VERSION) {
        qDebug("incorrect version");
        return false;
    }
    int count = 0;
    file.read(&count, 1);
    for (int i=0; i < count; ++i) {
        int k = 0;
        file.read(&k, 1);
        int v = 0;
        file.read(&v, 4);
        m_attri[k] = v;
    }

    file.read(&count, 1);
    for (int i=0; i < count; ++i) {
        int k = 0;
        file.read(&k, 1);
        std::string v;
        file >> v;
        m_attrs[k] = v;
    }
    return true;
}

const char * CDisplay::templateStr()
{
    return gets(DI_TEMPLATE);
}

void CDisplay::setTemplate(const char *s)
{
    set(DI_TEMPLATE, s);
}

void CDisplay::setFlagXY(int flagX, int flagY)
{
    set(DI_FLAG_X, flagX);
    set(DI_FLAG_Y, flagY);
}

bool CDisplay::isProtected()
{
    return geti(DI_PROTECTED) ? true : false;
}

void CDisplay::setProtected(bool b)
{
    set(DI_PROTECTED, b ? 1 : 0);
}

void CDisplay::setName(const char *name)
{
    set(DI_NAME, name);
}

bool CDisplay::shadow()
{
    return geti(DI_SHADOW) ? true : false;
}

int CDisplay::shadowX()
{
    return geti(DI_SHADOWX);
}

int CDisplay::shadowY()
{
    return geti(DI_SHADOWY);
}

int CDisplay::shadowR()
{
    return geti(DI_SHADOWR);
}

int CDisplay::shadowG()
{
    return geti(DI_SHADOWG);
}

int CDisplay::shadowB()
{
    return geti(DI_SHADOWB);
}

int CDisplay::shadowA()
{
    return geti(DI_SHADOWA);
}

int CDisplay::imageSet()
{
    return geti(DI_IMAGESET);
}

int CDisplay::imageNo()
{
    return geti(DI_IMAGENO);
}

int CDisplay::flagX()
{
    return geti(DI_FLAG_X);
}

int CDisplay::flagY()
{
    return geti(DI_FLAG_Y);
}

int CDisplay::geti(unsigned i)
{
    return m_attri[i];
}

const char* CDisplay::gets(int i)
{
    return m_attrs[i].c_str();
}

void CDisplay::set(int i, int v)
{
    m_attri[i] = v;
}

void CDisplay::set(int i, const char * v)
{
    m_attrs[i] = v;
}
