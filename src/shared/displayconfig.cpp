#include "displayconfig.h"
#include <string.h>
#include "stdafx.h"
#include "Display.h"
#include "IFile.h"

CDisplayConfig::CDisplayConfig()
{
    m_size = 0;
    m_max = MAX;
    m_displays = new CDisplay*[MAX];
}

CDisplayConfig::~CDisplayConfig()
{
    forget();
    delete [] m_displays;
}

int CDisplayConfig::indexOf(const char *name)
{
    for (int i=0; i < m_size; ++i) {
        if (!strcmp(m_displays[i]->name(), name)) {
            return i;
        }
    }
    return NOT_FOUND;
}

CDisplay * CDisplayConfig::find(const char *name)
{
    int i = indexOf(name);
    if (i != NOT_FOUND) {
        return m_displays[i];
    } else {
        return NULL;
    }
}

void CDisplayConfig::removeAt(int i)
{
    if (i >= 0 && i < m_size) {
        delete m_displays[i];
        for (int j = i + 1; j < m_size; ++j) {
            m_displays[j-1] = m_displays[j];
        }
        -- m_size;
    }
}

void CDisplayConfig::resize()
{
    if (m_size == m_max) {
        m_max += GROWBY;
        CDisplay **tmp = new CDisplay*[m_max];
        memcpy(tmp, m_displays, sizeof(CDisplay*) * m_size);
        delete [] m_displays;
        m_displays = tmp;
    }
}

CDisplay & CDisplayConfig::add(CDisplay & display)
{
    CDisplay *newDisplay;
    int i = indexOf(display.name());
    if (i == NOT_FOUND) {
        // create new display
        newDisplay = new CDisplay;
        *(newDisplay) = display;
        i = m_size;
        m_displays[i] = newDisplay;
        ++m_size;
        resize();
    } else {
        // reuse existing display
        *(m_displays[i]) = display;
    }
    return *(m_displays[i]);
}

CDisplay & CDisplayConfig::add(const char *name, int x, int y, int type)
{
    CDisplay display(name, x, y, type);
    return add(display);
}

int CDisplayConfig::getSize()
{
    return m_size;
}

void CDisplayConfig::forget()
{
    for (int i=0; i < m_size; ++i) {
        delete m_displays[i];
        m_displays[i] = NULL;
    }
    m_size = 0;
}

void CDisplayConfig::reset()
{
    // clear everything add the engine default
    forget();

    CDisplay & display1 = add("timeLeft", - 64, 0, CDisplay::DISPLAY_TIME_LEFT);
    display1.setColor(0x90, 0x50, 0x80, 0xf0);
    display1.setFontSize(30);
    display1.setVisible(true);
    display1.setShadow(true, 4, 4);
    display1.setProtected(true);

    CDisplay & display2 = add("score", 0, 0, CDisplay::DISPLAY_SCORE);
    display2.setColor(222, 222, 222, 0xf0);
    display2.setFontSize(26);
    display2.setVisible(true);
    display2.setShadow(true, 1, 1);
    display2.setProtected(true);

    CDisplay & display3 = add("debug", 8, 0, CDisplay::DISPLAY_DEBUGX);
    display3.setColor(0xff, 0xff, 0x00, 0x40);
    display3.setFontSize(16);
    display3.setVisible(false);
    display3.setProtected(true);

    CDisplay & display4 = add("pause", -1, -1, CDisplay::DISPLAY_MESSAGE);
    display4.setColor(0x00, 0xff, 0xff, 0x80);
    display4.setFontSize(40);
    display4.setVisible(false);
    display4.setText("PRESS F4", CDisplay::DISPLAY_SAME);
    display4.setShadow(true, 4, 4);
    display4.setProtected(true);

    CDisplay & display5 = add("lives", -1, 0, CDisplay::DISPLAY_LIVES);
    display5.setColor(0xff, 0xff, 0xFF, 0x40);
    display5.setFontSize(26);
    display5.setTemplate("Lives %.2d");
    display5.setVisible(false);
    display5.setProtected(true);
}

bool CDisplayConfig::read(IFile & file)
{
    reset();
    unsigned int file_version = 0;
    file.read(&file_version, sizeof(file_version));
    if(file_version != VERSION) {
        qDebug("incorrect version CDisplayConfig");
        return false;
    }
    int size;
    file.read(&size, sizeof(size));
    for (int i=0; i < size; ++i) {
        CDisplay display;
        display.read(file, file_version);
        add(display);
    }
    return true;
}

bool CDisplayConfig::write(IFile & file)
{
    unsigned int version = VERSION;
    file.write(&version, sizeof(version));
    file.write(&m_size, sizeof(m_size));
    for (int i=0; i < m_size; ++i) {
        m_displays[i]->write(file);
    }
    return true;
}

CDisplay *CDisplayConfig::operator[](int i)
{
    if (i >= 0 && i < m_size) {
        return m_displays[i];
    } else {
        return NULL;
    }
}

CDisplay *CDisplayConfig::operator[](const char *s)
{
    return find(s);
}

void CDisplayConfig::killFrameSet(int frameSet)
{
    for (int i=0; i < m_size; ++i) {
        CDisplay * d = m_displays[i];
        int is = d->imageSet();
        if (is == frameSet) {
            d->setImage(0,0,false);
        } else {
            d->setImage(--is, d->imageNo(), false);
        }
    }
}
