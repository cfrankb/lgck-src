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

// TextArray.cpp : implementation file
//

#include <QDebug>
#include "TextArray.h"

/////////////////////////////////////////////////////////////////////////////
// CTextArray

CTextArray::CTextArray()
{
    Init();
}

CTextArray::~CTextArray()
{
    Forget();
}

/////////////////////////////////////////////////////////////////////////////
// CTextArray message handlers

void CTextArray::Init()
{
    m_nSize = 0;

    m_pHead = NULL;
    m_pTail = NULL;
}

void CTextArray::Forget()
{
    CItem *p = m_pHead;
    CItem *pp;

    while (p)
    {
        pp = p->m_pNext;
        delete p;
        p = pp;
    }

    m_pHead = NULL;
    m_pTail = NULL;
    m_nSize = 0;
}

bool CTextArray::Add(const QString str)
{
    if (m_pHead)
    {
        CItem *pItem = new CItem(str, m_pTail);
        m_pTail->m_pNext = pItem;
        m_pTail = pItem;
    }
    else
    {
        CItem *pItem = new CItem(str);
        m_pHead = pItem;
        m_pTail = pItem;
    }

    m_nSize ++;

    return TRUE;
}

QString & CTextArray::operator[] (int n)
{
    static QString str = "[bad index]";

    if (n<0 || n>=m_nSize)
    {
        qDebug() << QString("CTextArray::operator[]; n=%1 out of bound\r\n").arg(n);

        return str;
    }
    else
    {
        CItem *p = m_pHead;
        while (n)
        {
            p = p->m_pNext;
            n--;
        }

        return p->m_str;

    }
}

int CTextArray::GetSize()
{
    return m_nSize;
}

QString CTextArray::readString(FILE *sfile) const
{
    int size = 0;
    fread(&size, 1, 1, sfile);
    //TODO: implement full 32bits
    if (size == 255) {
        fread(&size, 2, 1, sfile);
    }

    char buf[size + 1];
    buf[size] = 0;
    fread(buf, size, 1, sfile);
    return QString(buf);
}

void CTextArray::writeString(FILE *tfile, QString str)
{
    int size = str.length();
    // TODO implement full 32 bits
    if (size < 255) {
        fwrite(&size, 1, 1, tfile);
    }
    else {
        char b = 255;
        fwrite(&b, 1, 1, tfile);
        fwrite(&size, 2, 1, tfile);
    }

    QByteArray ba = str.toAscii();
    fwrite(ba.constData(), size, 1, tfile);
}

void CTextArray::read(FILE *sfile)
{
    // read

    int nSize;
    QString str;
    Forget();

    //ar >> nSize;
    fread(&nSize, 4, 1, sfile);

    while (nSize) {
//        ar >> str;
        str = readString(sfile);
        Add(str);
        nSize--;
    }
}

void CTextArray::write(FILE *tfile)
{
    // write
//    ar << m_nSize;
    fwrite(&m_nSize, 4, 1, tfile);
    CItem *p = m_pHead;

    while (p) {
        //ar << p->m_str;
        writeString(tfile, p->m_str);
        p = p->m_pNext;
    }
}

CTextArray & CTextArray::operator = (CTextArray & src)
{
    Forget();

    CItem *p = src.m_pHead;

    while (p)
    {
        Add(p->m_str);
        p = p->m_pNext;
    }

    return *this;
}

void CTextArray::RemoveAt(int i)
{
    if (i<0 || i>=m_nSize) {
        qDebug() << "out of bound\n";
        return ;
    }

    CItem *pPrev = NULL;
    CItem *p = m_pHead;

    while (i && p) {
        pPrev = p;
        p = p->m_pNext;

        i--;
    }

    if (pPrev) {
        pPrev->m_pNext = p->m_pNext;
        if (p->m_pNext == NULL) {
            m_pTail = pPrev;
        }
    } else {
        m_pHead = p->m_pNext;
        if (m_pHead == NULL) {
            m_pTail = NULL;
        }
    }

    delete p ;
}
