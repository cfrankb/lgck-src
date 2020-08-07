/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2005, 2011  Francois Blanchette

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

#include <QFile>
#include "OBL5File.h"
#include "../shared/Frame.h"

//#define q2c(__qstring__) (const char*) __qstring__.toAscii().constData()
#define q2c(__qstring__) (const char*) __qstring__.toLatin1().data()

COBL5File::COBL5File()
{
    init();
}

COBL5File::~COBL5File()
{
}

void COBL5File::init()
{
    strcpy(m_format, "OBL5");
    m_dirty = false;
    m_currImage = -1;
}

void COBL5File::forget()
{
    m_filter.forget();
    m_fileName = "";
    m_lastError = "";
    m_currImage = -1;
}

bool COBL5File::read()
{
    bool result = false;
    CFileWrap file;
    if (file.open(q2c(m_fileName), "rb")) {
        CFrameSet set;
        char format[7];
        if (set.extract(file,format) && set.getSize()) {
            result = true;
            m_filter.forget();
            for (int i=0; i < set.getSize(); ++i){
                m_filter.add(new CFrame(set[i]));
            }
            m_currImage = 0;
            if (CFrameSet::isFriendFormat(format)) {
                strcpy(format, "OBL5");
            }
            setFormat(format);
        } else {
            m_lastError = "can't extract images";
            m_currImage = -1;
        }
        file.close();
    } else {
        m_lastError = "can't open file";
        m_currImage = -1;
    }

    return result;
}

bool COBL5File::write()
{
    bool result = false;
    CFileWrap file;
    if (file.open(q2c(m_fileName), "wb")) {
        result = true;        
        if (strcmp(m_format, "OBL5")==0) {
            m_filter.write(file);
        } else {
            unsigned char *data;
            int size;
            m_filter.toPng(data, size);
            file.write(data, size);
            delete[] data;
        }
        file.close();
    } else {
        m_lastError = "can't write file";
    }

    return result;
}

QString COBL5File::getFileName() const
{
    return m_fileName;
}

QString COBL5File::getLastError() const
{
    return m_lastError;
}

void COBL5File::setFileName(const QString fileName)
{
    m_fileName = fileName;
}

bool COBL5File::isUntitled()
{
    return m_fileName.isEmpty();
}

bool COBL5File::isDirty()
{
    return m_dirty;
}

void COBL5File::setDirty(bool b)
{
    m_dirty = b;
}

int COBL5File::getSize()
{
    return m_filter.getSize();
}

CFrameSet & COBL5File::getImageSet()
{
    return m_filter;
}

void COBL5File::add ( CFrame * frame )
{
    m_filter.add(frame);
}

CFrame * COBL5File::operator [] (int i)
{
    return m_filter[i];
}

CFrame * COBL5File::getCurrent() const
{
    if (m_currImage >= 0) {
        return m_filter[m_currImage];
    } else {
        return nullptr;
    }
}

void COBL5File::setCurrentIndex(int curr)
{
    m_currImage = curr;
}

int COBL5File::getCurrentIndex()
{
    return m_currImage;
}

const char *COBL5File::getFormat()
{
    return m_format;
}

void COBL5File::setFormat(const char *format)
{
    strcpy(m_format, format);
}
