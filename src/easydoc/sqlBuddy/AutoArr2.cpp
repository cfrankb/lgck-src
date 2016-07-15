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

#include <QDebug>
#include "AutoArr2.h"

/////////////////////////////////////////////////////////////////////
// constructor / destructor

CAutoArr2::AA_ITEM CAutoArr2::m_aaitem;
bool CAutoArr2::m_bAutoCreate = FALSE;

CAutoArr2::CAutoArr2()
{
    Init();
}

CAutoArr2::~CAutoArr2()
{
    Forget();
}

void CAutoArr2::Init()
{
    m_nSize = 0;
    m_block.Init();
    m_pActive = &m_block;
}

void CAutoArr2::Forget()
{
    AA_BLOCK *p = (AA_BLOCK*) m_block.GetNext();
    while (p)
    {
        AA_BLOCK *pp = (AA_BLOCK*) p->GetNext();
        delete p;
        p = pp;
    }

    Init();
}

void CAutoArr2::RemoveAll()
{
    Forget();
}

/////////////////////////////////////////////////////////////////////
// attributes

bool CAutoArr2::Exists(QString strName)
{
    CAutoArr2 & arr = * this;
    int i;
    for (i=0; i<m_nSize; i++)
    {
        AA_ITEM & item = arr[i];
        if (item.GetName() == strName)
        {
            return TRUE;
        }
    }

    return FALSE;
}

bool CAutoArr2::Exists(int nId)
{
    CAutoArr2 & arr = * this;
    int i;
    for (i=0; i<m_nSize; i++)
    {
        AA_ITEM & item = arr[i];
        if (item.GetId() == nId)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/////////////////////////////////////////////////////////////////////
// operators

CAutoArr2::AA_ITEM & CAutoArr2::operator[] (int i)
{
    //ASSERT(i>=0 && i<m_nSize);

    if (i>=0 && i<m_nSize)
    {
        int nBase;
        AA_BLOCK *pBlock = FindBlock(i, nBase);
        if (pBlock)
        {
            return (*pBlock)[i-nBase];
        }
        else
        {
            return m_aaitem;;
        }
    }
    else
    {
        return m_aaitem;;
    }
}

CAutoArr2::AA_ITEM & CAutoArr2::operator[] (QString strName)
{   
    CAutoArr2 & arr = * this;

    for (int i=0; i<m_nSize; i++)
    {
        AA_ITEM & item = arr[i];
        if (item.GetName() == strName)
        {
            return item;
        }
    }

    if (m_bAutoCreate)
    {
        return Add(strName);
    }

    return m_aaitem;;
}

CAutoArr2 & CAutoArr2::operator = (CAutoArr2 & s)
{
    if (this != & s)
    {

        int i;
        Forget();
        for (i=0; i<s.GetSize(); i++)
        {
            Add(s[i]);
        }
    }

    return * this;
}

CAutoArr2 & CAutoArr2::operator += (CAutoArr2 & s)
{
    int i;
    int c = s.GetSize();
    for (i=0; i < c; i++)
    {
        Add(s[i]);
    }

    return * this;
}

CAutoArr2 & CAutoArr2::operator += (CAutoArr2::AA_ITEM & item)
{
    Add(item);
    return * this;
}

/////////////////////////////////////////////////////////////////////
// operations

CAutoArr2::AA_ITEM & CAutoArr2::Add(AA_ITEM & item)
{
    if (m_pActive->GetSize()==AA_BLOCK_SIZE)
    {
        AA_BLOCK *pNew = new AA_BLOCK;
        //ASSERT(pNew);

        m_pActive->SetNext( pNew );
        m_pActive = pNew;
    }

    AA_ITEM & t_item = m_pActive->Add(item);
    m_nSize++;
    return t_item;
}

CAutoArr2::AA_ITEM & CAutoArr2::Add(QString strName, QString strContent, int nId, int nFK)
{
    AA_ITEM item (strName, strContent, nId, nFK);
    return Add(item);  
}

bool CAutoArr2::InsertAt(int i, AA_ITEM & item)
{
    bool bStatus = FALSE;

    if (i>=0 || i<m_nSize)
    {
        int nBase;
        AA_BLOCK *p = FindBlock(i, nBase);
        if (p)
        {
            // block is full
            // create a new block
            if (p->GetSize()==AA_BLOCK_SIZE)
            {
                AA_BLOCK *pNew = new AA_BLOCK;
                pNew->SetNext(p->GetNext());
                pNew->Add((*p)[AA_BLOCK_SIZE-1]);

                p->SetNext(pNew);
                if (m_pActive==p)
                {
                    m_pActive = pNew;
                }
            }

            int n;

            for (n = qMin(p->GetSize(), AA_BLOCK_SIZE - 1); n > (i - nBase); n --)
            {
                (*p)[n] = (*p)[n-1];
            }

            (*p)[i - nBase] = item;

            
            if (p->GetSize() != AA_BLOCK_SIZE)
            {
                p->Add();
            }                               

            m_nSize++;
            bStatus = TRUE;

        }
    }
    else
    {
        if (i == m_nSize)
        {
            Add(item);
            bStatus = TRUE;
        }
    }

    return bStatus;
}

void CAutoArr2::RemoveAt(int i)
{
    if (i>=0 || i<m_nSize)
    {
        int n, nBase;
        if (AA_BLOCK *p = FindBlock(i, nBase))
        {
            for (n = i - nBase;  n < (p->GetSize()-1);  n++)
            {
                (*p) [n] = (*p) [n + 1];
            }

            p->Remove();
            m_nSize--;
        }
    }
}

int CAutoArr2::Remove(QString str)
{
    int s=0;
    int t=0;
    int i=0;
    int x=0;

    AA_BLOCK *p = & m_block;
    while (p)
    {
        x = 0;
        t = 0;
        for (s=0; s<p->GetSize(); s++)
        {
            AA_ITEM & item_s = (*p)[s];
            if (item_s.GetName() == str)
            {
                i++;
                x++;
            }
            else
            {
                if (s != t)
                {
                    AA_ITEM & item_t = (*p)[t]; 
                    item_t = item_s;
                }

                t++;
            }
        }

        p->Remove(x);
        p = p->GetNext();
    }

    m_nSize -= i;
    return i;
}

int CAutoArr2::Rename(QString strOld, QString strNew)
{  
    int n=0;
    int i;

    CAutoArr2 & arr = * this;
    
    for (i=0; i<m_nSize; i++)    
    {
        AA_ITEM & item = arr[i];
        if (item.GetName() == strOld)
        {
            item.Rename(strNew);
            n++;
        }
    }

    return n;
}

int CAutoArr2::IndexOf(QString strName)
{   
    CAutoArr2 & arr = * this;

    for (int i=0; i<m_nSize; i++)
    {
        AA_ITEM & item = arr[i];
        if (item.GetName() == strName)
        {
            return i;
        }
    }

    return AA_ERROR;

}

int CAutoArr2::IndexOf(int nId)
{
    CAutoArr2 & arr = * this;

    for (int i=0; i<m_nSize; i++)
    {
        AA_ITEM & item = arr[i];
        if (item.GetId() == nId)
        {
            return i;
        }
    }

    return AA_ERROR;

}

/////////////////////////////////////////////////////////////////////
// private

CAutoArr2::AA_BLOCK * CAutoArr2::FindBlock(int i, int & nBase)
{
    AA_BLOCK *pBlock = NULL;
    nBase = 0;
    
    if (i>=0 && i<m_nSize)
    {
        pBlock = &m_block;
        int nMax = pBlock->GetSize();
        while (pBlock && (i>=nMax))
        {
            nBase += pBlock->GetSize();
            pBlock = pBlock ->GetNext();
            if (pBlock)
            {
                nMax += pBlock->GetSize();
            }
        }       
    }

    return pBlock;
}

/////////////////////////////////////////////////////////////////////
// static

int CAutoArr2::str2int(const QString str)
{
    int n = 0;

    for (int i=0; i < str.length() && str[i].digitValue() != -1; i++)
    {
        n = n * 10 + str[i].digitValue();
    }

    return n;
}

QString CAutoArr2::EGBDecode(const QString str)
{   
    QString tmp;

    for (int i=0; i < str.length(); i++)
    {
        if(str[i]=='\\')
        {
            i++;
            switch(str[i].toAscii())
            {
                case 't':
                    tmp += '\t';
                break;

                case 'r':
                    tmp += '\r';
                break;

                case 'n':
                    tmp += '\n';
                break;

                case '\'':
                    tmp += '\'';
                break;

                case '\"':
                    tmp += '\"';
                break;

                case '0':
                    tmp += '\0';
                break;

                default:
                    tmp += "\\" + str[i].toAscii();
                break;

            }
                
            i++;
        }
        else
        {
            tmp += str[i];
        }
    }


    return tmp;
}


/////////////////////////////////////////////////////////////////////
// debug


void CAutoArr2::EnumBlocks()
{
    QString str;

    int i=0;
    int n=0;
    int x=0;

    AA_BLOCK *p = &m_block;

    qDebug() << "\r\n\r\nDump begins\r\n\r\n";

    while (p)
    {
        QString str = QString("*** block %1; size=%2\n"). arg(QString::number(n)) . arg(QString::number(p->GetSize()));

        qDebug() << str;

        for (i=0; i<p->GetSize(); i++)
        {
            AA_ITEM & item = (*p)[i];

            str = QString("  <%1> [%2|%3] \"%4\" @%5@ (%6)\r\n")
                .arg(QString::number(i), QString::number(item.GetId()), QString::number(item.GetFK()), item.GetName(), item.GetContent(), QString::number(x));

            qDebug() << str;
            x++;
        }

        p = p->GetNext();    
        n++;
    }
}
