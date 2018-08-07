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

#include "stdafx.h"
#include "../shared/qtgui/cheat.h"
#include "FileWrap.h"

CFileWrap::CFileWrap()
{
}

CFileWrap::~CFileWrap()
{
    close();
}

CFileWrap & CFileWrap::operator >> (int & n)
{
    read(&n, 4);
    return *this;
}

CFileWrap & CFileWrap::operator << ( int n)
{
    write(&n, 4);
    return *this;
}

CFileWrap & CFileWrap::operator >> ( QString & str)
{   
    int x = 0;
    read (&x, 1);
    if (x == 0xff) {
        read (&x, 2);

        // TODO: implement 32 bits version
    }

    if (x != 0) {
        char *sz = new char[x + 1];
        sz [ x ] = 0;
        file.read (sz, x);
        str = QString(sz);
        delete [] sz;
    }
    else {
        str = "";
    }

    return *this;
}

CFileWrap & CFileWrap::operator << (QString str)
{
    int x = str.length();
    if (x <= 0xfe) {
        write (&x, 1);
    }
    else {
        int t = 0xff;
        write (&t, 1);
        write (&x, 2);

        // TODO : implement 32bits version
    }

    if (x!=0) {
        write(q2c(str), x);
    }

    return *this;
}

CFileWrap & CFileWrap::operator += (QString str)
{
    write(q2c(str), str.length());
    return *this;
}

CFileWrap & CFileWrap::operator &= (QString str)
{

    enum {
        fnUndefined     =   1,
        fnFinal         =   2,
        fnTBA           =   3,
        fnUntested      =   4,
        hdr0            =   5,
        hdr1            =   6,
        hdr2            =   7,
        hdr3            =   8,
    };

    QString style[] = {
        "",
        "fnUndef",
        "fnFinal",
        "fnTba",
        "fnUntested",
        "hdr0",
        "hdr1",
        "hdr2",
        "hdr3"
    };

    int o = 0;
    QString s1;
    bool breakup = false;
    for (int i=0, c=0, s=0, d=0; i < str.length(); ++i, ++c) {
        if (str[i]== '\n' && (!i || (str[i] != '\r'))) {
            if (d) {
                QString t = "</div>";
                s1 +=t;
                s+= t.length();
            }
            c = -1;
            s1 += "\r\n";
            s +=2;

            breakup= false;
            d = 0;
        } else {

            QString t;
            int x = 0;
            if (c == 0 && !breakup) {

                switch (str[i].toLatin1()) {

                case '*':
                    //qDebug("*");
                    if (str[i + 1]=='*' &&
                        str[i + 2]=='*') {
                        d = hdr0;
                        o = 2;
                        if (str[i + 3]==' ') {
                            ++o;
                        }
                    }
                    break;

                case '+':

                    //qDebug("+");
                    o = 0;
                    d = hdr1;
                    if (str[i + 1] == '+') {
                        ++o;
                        ++d;
                        if (str[i + 2] == '+') {
                            ++o;
                            ++d;
                            if (str[i + 3] == '+') {
                                d = 0;
                            }
                        }
                    }

                    break;

                case '?':
                    d = fnUndefined;
                    break;

                case '%':
                    d = fnFinal;
                    break;

                case '@':
                    d = fnTBA;
                    break;

                case '~':
                    d = fnUntested;
                    break;

                case '=':
                    d = -1;
                    o = 1;
                    switch (str[i + 1].toLatin1()) {
                        case '{':
                            t = "<div class=code>";
                        break;

                        case '}':
                            t = "</div>";
                        break;

                        default:
                            d = 0;
                            o = 0;
                        break;
                    }


                default:
                    //qDebug("%c %d %d", str[i].toAscii(), str[i].toAscii(), '+');
                    break;
                }

                x = d;
            }

            if (!x) {
                s1 += str[i];
                ++ s;
            } else {
                i += o;

                if (d != -1) {
                    t = QString("<div class=%1>").arg(style[d]);
                } else {
                   d = 0;
                }
                s1 += t;
                s += t.length();
            }
        }

        if (c == 80) {
            while (str[i].isLetterOrNumber() && i != 0) {
                --i;
                --s;
            }
            if (str[i].isLetterOrNumber()) {
                --i;
            }

            s1 = s1.mid(0, s) += "\r\n" ;

            s +=2;
            c = 0;

            breakup = true;
        }
    }


    write(q2c(s1), s1.length());
    return *this;
}


CFileWrap & CFileWrap::operator += (const char *s)
{
    write(s, strlen(s));
    return *this;
}

bool CFileWrap::open(const QString fileName,  QIODevice::OpenMode mode)
{
    file.setFileName(fileName);
    return file.open(mode);
}

void CFileWrap::close()
{
    if (file.openMode() != QIODevice::NotOpen){
        file.close();
    }
}

long CFileWrap::getSize()
{
    return file.size();
}

void CFileWrap::seek(long p)
{
    file.seek(p);
}

long CFileWrap::tell()
{
    return file.pos();
}

CFileWrap & CFileWrap::operator >> (QStringList & list)
{
    list.empty();
    int size=0;
    read(&size, sizeof(size));
    for (int i=0; i < size; ++i) {
        QString str;
        *this >> str;
        list.append(str);
    }
    return *this;
}

CFileWrap & CFileWrap::operator << (const QStringList & list)
{
    int size = list.size();
    write(&size, sizeof(size));
    QListIterator<QString> itr (list);
    while (itr.hasNext()) {
        QString current = itr.next();
        *this << current;
        //qDebug() << "{" <<  current << "}";
    }
    return *this;
}

int CFileWrap::read(void *buf, int size)
{
    return file.read( (char*) buf, size);
}


bool CFileWrap::open(const char *s, const char *mode)
{
    return open(QString(s));
}
