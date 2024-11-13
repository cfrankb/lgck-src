/*
    easyDoc - documentation utility for LGCK Builder
    Copyright (C) 2009, 2010  Francois Blanchette

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


#ifndef FILEWRAP_H
#define FILEWRAP_H

#include <QFile>

class CFileWrap
{
public:

    CFileWrap();
    ~CFileWrap();

    CFileWrap & operator >> (QString & str);
    CFileWrap & operator << (QString str);
    CFileWrap & operator &= (QString str);

    CFileWrap & operator >> (int & n);
    CFileWrap & operator << (int n);

    CFileWrap & operator += (QString str);
    CFileWrap & operator += (const char *);

    CFileWrap & operator >> (QStringList & list);
    CFileWrap & operator << (const QStringList & list);

    bool open(const QString,  QIODevice::OpenMode mode=QIODevice::ReadOnly);
    bool open(const char *s, const char *mode);
    int read(void *buf, int size);
    int write(const void *buf, int size) { return file.write( (char*) buf, size); }
    void close();
    long getSize();
    void seek(long i);
    long tell();

protected:

    QFile file;
};

#endif // FILEWRAP_H
