/*
    LGCK Builder package
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

#ifndef OBL5FILE_H
#define OBL5FILE_H

#include "../shared/FrameSet.h"
#include <QString>

class CFrame;

class COBL5File
{
public:
    COBL5File();
    ~COBL5File();

    void add ( CFrame * frame );
    void init();
    void forget();
    bool read();
    bool write();
    QString getFileName() const;
    void setFileName(const QString fileName);
    bool isUntitled();
    bool isDirty();
    void setDirty(bool b);
    QString getLastError() const;
    int getSize();
    CFrameSet & getImageSet();
    CFrame * operator [] (int i);
    CFrame * getCurrent() const;
    void setCurrentIndex(int curr);
    int getCurrentIndex();
    const char *getFormat();
    void setFormat(const char *format);

protected:
    QString m_fileName;
    QString m_name;
    QString m_lastError;
    bool m_dirty;
    int m_currImage;
    char m_format[7];
    CFrameSet m_filter;
};

#endif // OBL5FILE_H
