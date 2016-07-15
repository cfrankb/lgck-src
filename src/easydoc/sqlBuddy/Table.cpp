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

// Table.cpp : implementation file
//

#include "Table.h"
#include <QDebug>

/////////////////////////////////////////////////////////////////////////////
// CTable

CTable::CTable()
{
    Init();
}

CTable::~CTable()
{
    Forget();
}

/////////////////////////////////////////////////////////////////////////////
// CTable message handlers

bool CTable::Add(CField * p)
{
    if (m_pTail)
    {
        m_pTail -> m_pNext = p;
    }

    if (!m_nSize)
    {
        m_pHead = p;
    }

    m_pTail= p;
    p -> m_pNext = NULL;
    m_nSize++;

    return TRUE;
}

bool CTable::RemoveAt(int n)
{
    if (m_pHead && m_pTail &&
        !(n<0) && !(n>=m_nSize))
    {
        if (n==0)
        {
            CField *p = m_pHead;
            m_pHead = m_pHead->m_pNext;
            m_nSize--;
            delete p;

            if (!m_pHead)
            {
                m_pTail = NULL;
            }
        }
        else
        {
            CTable & table = (*this);
            CField *p = table[n];
            if (p)
            {
                if (p == m_pTail)
                {
                    CField *pp = table[n-1];
                    m_pTail = pp;
                    pp->m_pNext = NULL;
                    delete p;
                }
                else
                {
                    CField *pp = table[n-1];
                    pp->m_pNext = table[n+1];
                    delete p;
                }
                m_nSize--;
            }
        }

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


CField * CTable::operator[] (int n)
{
    if (n<0 || n>=m_nSize)
    {
        return NULL;
    }
    else
    {
        if (!m_pHead)
        {
            return NULL;
        }
        else
        {
            CField *p = m_pHead;
            while (n && p)
            {
                p = p->m_pNext;
                n --;
            }

            return p;
        }
    }
}

void CTable::Init()
{
    m_strName = "";

    m_pNext = NULL;
    m_pHead = NULL;
    m_pTail = NULL;

    m_szInfo[0] = 0 ;
    m_nSize = 0;
    m_pWnd = NULL;

    m_nX = 100;
    m_nY = 100;
    m_nLen = 100;
    m_nHei = 100;
}

void CTable::Forget()
{
    CField * p = m_pHead;

    while (p)
    {
        CField * pp = p;
        p = p -> m_pNext;
        delete pp ;
    }

    Init();
}

QString CTable::readString(FILE *sfile) const
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

void CTable::writeString(FILE *tfile, QString str)
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


void CTable::read(FILE *sfile, qint32 nVer)
{
    Forget();
    qint32 nSize=0;
    //ar.Read(&nSize,4);
    fread(&nSize, 4, 1, sfile);

    if (nVer < 4)
    {
        // for legacy files
        char szName[MAX_TB_NAME];
        //ar.Read(szName, MAX_TB_NAME);
        fread(szName,  MAX_TB_NAME, 1, sfile);
        SetName(szName);
    }
    else
    {
        //ar >> m_strName;
        m_strName = readString(sfile);
    }

    //ar.Read(&m_nX,4);
    //ar.Read(&m_nY,4);
    //ar.Read(&m_nLen,4);
    //ar.Read(&m_nHei,4);
    fread(&m_nX,4, 1, sfile);
    fread(&m_nY,4, 1, sfile);
    fread(&m_nLen,4, 1, sfile);
    fread(&m_nHei,4, 1, sfile);

    m_nX--;
    m_nY--;

    CField *p = NULL;
    m_pHead = NULL;

    for (int i=0; i<nSize; i++)
    {
        p = new CField;
        p->Init();
        //p->Serialize(ar, nVer);
        p->read(sfile, nVer);
        Add(p);
    }

    int nInfoSize;
    switch (nVer)
    {
    case 0:
    case 1:
    case 2:
        m_szInfo[0]=0;
        break;

    case 3:
    case 4:
        //ar.Read(&nInfoSize, 4);
        fread(&nInfoSize, 4,1, sfile);
        //ar.Read(m_szInfo, nInfoSize);
        fread(m_szInfo, nInfoSize, 1, sfile);
        m_szInfo[nInfoSize]=0;
        break;
    }

    if (nVer < 4)
    {
        FixLegacy();
    }

    m_pWnd = NULL;
    m_pNext = NULL;

}

void CTable::write(FILE *tfile)
{
    //ar.Write(&m_nSize, 4);
    //ar << m_strName;
    fread(&m_nSize, 4, 1, tfile);
    writeString(tfile, m_strName);

    //ar.Write(&m_nX,4);
    //ar.Write(&m_nY,4);
    //ar.Write(&m_nLen,4);
    //ar.Write(&m_nHei,4);

    fwrite(&m_nX, 4, 1, tfile);
    fwrite(&m_nY, 4, 1, tfile);
    fwrite(&m_nLen, 4, 1, tfile);
    fwrite(&m_nHei, 4, 1, tfile);

    CTable & table = (*this);

    for (int i=0; i<m_nSize; i++) {
        CField *p = table[i];
        if (p) {
            //p->Serialize(ar, nVer);
            p->write(tfile);
        }
    }

    // version 0.3 & up
    int nInfoSize = strlen(m_szInfo);
    //ar.Write(&nInfoSize, 4);
    //ar.Write(m_szInfo, nInfoSize);

    fwrite(&nInfoSize, 4, 1, tfile);
    fwrite(m_szInfo, nInfoSize,1, tfile);
}

int CTable::GetSize()
{
    return m_nSize;
}

QString CTable::sql(const QString tableName)
{
    QString strSQL;
    CTable & table = (*this);

    // overide the default table name
    if (tableName != "") {
        strSQL = "CREATE TABLE " + table.GetName() + " (\r\n";
    } else {
        strSQL = "CREATE TABLE " + tableName + " (\r\n";
    }

    for (int i=0; i<table.GetSize(); i++) {
        CField *p = table[i];
        if (p) {
            strSQL = strSQL + "    " +  p->formated();
            if ((i+1)!=table.GetSize()) {
                strSQL += ",\r\n";
            }
            else {
                strSQL += "\r\n";
            }
        }
    }

    return strSQL += ");";
}


void * CTable::GetWndTable()
{
    return m_pWnd;
}

void CTable::SetWndTable(void *pWnd)
{
    m_pWnd = pWnd;
}

void CTable::SetWndSize(int nLen, int nHei)
{
    m_nLen = nLen;
    m_nHei = nHei;
}

void CTable::SetWndXY(int nX, int nY)
{
    m_nX = nX;
    m_nY = nY;
}

void CTable::GetWndSize(int & nLen, int &nHei)
{
    nLen = m_nLen;
    nHei = m_nHei;
}

void CTable::GetWndXY(int & nX, int &nY)
{
    nX = m_nX;
    nY = m_nY;
}

const QString CTable::techDoc()
{
    CTable & current = *this;
    QString str = QString("\r\n<p><b>")
          + current.GetName()
          + "</b></p><p><table border=1>\r\n" ;

    qDebug() << current.GetName() << "\r\n";

    for (int i=0; i<m_nSize; i++) {
        if (!current[i]->IsAlias()) {
            str += current[i]->techDoc();
        }
    }
    return "\r\n</table>\r\n"
           + QString("<p>") + QString(m_szInfo) + "\r\n";
}

CField * CTable::FindField(const QString field)
{
    CTable & current = *this;
    for (int i=0; i<m_nSize; i++) {
        CField *p = current[i];
        if (p->GetName() == field) {
            return p;
        }
    }

    return NULL;    
}

void CTable::SetName (QString strName)
{
    m_strName = strName;
}

QString & CTable::GetName()
{
    return m_strName;
}

void CTable::FixLegacy()
{
    int nSize = m_nSize;

    CTable & current = *this;
    for (int i=0; i<nSize; i++) {
        CField *p = current[i];

        int nType = p->GetType();
        if (nType< 0x100) {
            if (p->m_bPrimary) {
                CField *pNew = new CField();
                pNew->SetName(p->GetName());
                pNew->SetType(0x100);
                pNew->GetFriends().Add(p->GetName());
                Add(pNew);
            }

            if (p->m_bUnique) {
                CField *pNew = new CField();
                pNew->SetName(p->GetName());
                pNew->SetType(0x101);
                pNew->GetFriends().Add(p->GetName());
                Add(pNew);
            }

            if (p->m_bKey) {
                CField *pNew = new CField();
                pNew->SetName(p->GetName());
                pNew->SetType(0x102);
                pNew->GetFriends().Add(p->GetName());
                Add(pNew);
            }
        }
    }
}

void CTable::InsertAt(CField * pField, int i)
{
    if (i<0 || i> m_nSize) {
        // can't insert item
        qDebug() << ("CTable::InsertAt(); can't insert item (invalid index)\n");
    }
    else {
        CField *p = m_pHead;
        CField *pPrev = NULL;

        while (i && p) {
            pPrev = p;
            p = p->m_pNext;

            i--;
        }

        if (pPrev) {
            pPrev->m_pNext = pField;
        } else  {
            m_pHead = pField;
        }

        pField->m_pNext = p;

        if (p == NULL) {
            m_pTail = pField;
        }

        m_nSize++;
    }
}

int CTable::FindFirstAlias()
{
    int i = 0;
    CField *p = m_pHead;

    while (p){
        if (p->IsAlias()) {
            return i;
        }

        p = p->m_pNext;
        i++;
    }

    return -1;
}
