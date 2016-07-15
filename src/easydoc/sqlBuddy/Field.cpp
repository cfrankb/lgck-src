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

// Field.cpp : implementation file
//

#include <QDebug>
#include "Field.h"

/////////////////////////////////////////////////////////////////////////////
// CField

CField::CField()
{
    Init();
}

CField::~CField()
{
}

/////////////////////////////////////////////////////////////////////////////
// CField message handlers

QString CField::m_arrTypes [] =
{
    "int",
    "tinyint",
    "mediumint",
    "bigint",
    "varchar",
    "char",
    "text", // no size
    "tinytext", // no size
    "date", // no size (0000-00-00)
    "datetime", // 	'0000-00-00 00:00:00'
    "enum", // special
    "double", // (x,y)
    "time",
    "mediumtext",
    "bigtext",
    "timestamp",
    "year",
    //"newdate",
    //"short",
    "set",
    "null",
    "boolean",
    "blob",
    "mediumblob",
    "smallint",
    "decimal",
    "float",
   ""
};

QString CField::m_arrTypeAlias[]=
{
    "longtext", "bigtext",
    "decimal", "double",
    "smallint", "tinyint",
    "blob", "text",
    "", ""
};

void CField::Init()
{
    SetName("");
    SetInfo("");
    SetDefault("");
    SetArgs("");

    m_bPrimary  = FALSE;
    m_bNotNull  = FALSE;
    m_bAuto = FALSE;
    m_bKey = FALSE;
    m_bUnique = FALSE;
    m_bUnsigned = FALSE;
    m_nType = 0;
    //	m_nSize = 0;
    m_pNext = NULL;
}

void CField::read(FILE *sfile, qint32 nVer)
{
    switch(nVer)
    {
        // legacy version loading code
    case 0:
    case 1:
    case 2:
    case 3:
        FIELD_INFO temp;
        fread(&temp, sizeof(FIELD_INFO), 1, sfile);
        SetName(temp.m_szName);
        SetDefault(temp.m_szDefault);
        SetArgs(temp.m_szArg);
        m_nType = temp.m_nType;

        m_bPrimary = temp.m_bPrimary;
        m_bNotNull = temp.m_bNotNull ;
        m_bAuto = temp.m_bAuto;
        m_bKey = temp.m_bKey;
        m_bUnique = temp.m_bUnique;
        m_bUnsigned = temp.m_bUnsigned;

        if (nVer < 3) {
            SetInfo("");
        }
        else {
            char szInfo[MAX_FI_INFO];
            int nInfoSize;
            fread(&nInfoSize, 4, 1, sfile);
            fread(szInfo, nInfoSize, 1, sfile);
            szInfo[nInfoSize]=0;
            SetInfo(szInfo);
        }
        break;

        // load data from latest revision

                        case 4:

        //ar >> m_strName;
        m_strName = readString(sfile);
        //ar >> m_strArgs;
        m_strArgs = readString(sfile);
        //ar >> m_strInfo;
        m_strInfo = readString(sfile);
        //ar >> m_strDefault;
        m_strDefault = readString(sfile);

        //ar >> m_nType;
        fread(&m_nType, 4, 1, sfile);
        m_arrFriends.read(sfile);

        fread(&m_bNotNull, 1, 1, sfile);
        fread(&m_bAuto, 1, 1, sfile);
        fread(&m_bUnsigned, 1, 1, sfile);
        break;

    default:
        qDebug() << "Serialized unknown field version " << nVer << "\n";
        break;
    }

    m_pNext = NULL;
}

void CField::write(FILE *tfile)
{
    //ar << GetName();
    //ar << GetArgs();
    //ar << GetInfo();
    //ar << GetDefault();
    writeString(tfile, m_strName);
    writeString(tfile, m_strArgs);
    writeString(tfile, m_strInfo);
    writeString(tfile, m_strDefault);

    //ar << m_nType;
    fwrite(&m_nType, 4, 1, tfile);
    m_arrFriends.write(tfile);

    //ar << (BYTE) m_bNotNull;
    //ar << (BYTE) m_bAuto;
    //ar << (BYTE) m_bUnsigned;
    fwrite(&m_bNotNull, 1, 1, tfile);
    fwrite(&m_bAuto, 1, 1, tfile);
    fwrite(&m_bUnsigned, 1, 1, tfile);
}

void CField::Forget()
{
    Init();
}

int CField::findType(const QString s)
{
    int i;
    for (i=0; m_arrTypes[i] != ""; i++) {
        if ( s == m_arrTypes[i] ) {
            return i;
        }
    }

    qDebug() << QString("Type %1 is unknown\n") . arg(s);
    return -1;
}

QString CField::techDoc() const
{
    QString s;
    QString strTemp;
    if (m_nType < FI_ALIAS_PRIMARY) {
        QString typeName;
        if (m_nType !=-1) {
            typeName = m_arrTypes[m_nType];
        }

        if (m_strArgs != "") {

            strTemp = QString("%1(%2)") . arg(typeName, m_strArgs);
        }
        else {
            strTemp = typeName;
        }

        s = QString("<tr>\r\n" \
                       "  <td valign=TOP width=212>%1</td>\r\n" \
                       "  <td valign=TOP width=212>%2</td>\r\n" \
                       "  <td valign=TOP width=212>%3</td>\r\n" \
                       "</tr>\r\n") .arg( m_strName, strTemp, m_strInfo);
    }

    return s;
}

QString CField::GetName() const
{
    return m_strName;
}

QString CField::GetArgs() const
{
    return m_strArgs;
}

QString CField::GetDefault() const
{
    return m_strDefault;
}

QString CField::GetInfo() const
{
    return m_strInfo;
}

void CField::SetName(QString strName)
{
    m_strName = strName;
}

void CField::SetArgs(QString strArgs)
{
    m_strArgs = strArgs;
}

void CField::SetDefault(QString strDefault)
{
    m_strDefault= strDefault;
}

void CField::SetInfo(QString strInfo)
{
    m_strInfo = strInfo;
}

const QString CField::formated()
{
    QString strTemp;

    if (m_nType >= FI_ALIAS_PRIMARY) {
        QString arrTypes[] = {
            "PRIMARY KEY",
            "UNIQUE KEY",
            "KEY"
        };

        if (m_nType & 3) {
            strTemp = QString("%1 %2").arg( arrTypes[m_nType & 3], GetName());
        }
        else {
            strTemp = arrTypes[m_nType & 3];
        }

        if (m_arrFriends.GetSize()) {
            strTemp += " (";

            int i;
            for (i=0; i < m_arrFriends.GetSize(); i++) {
                if (i != 0) {
                    strTemp += ", ";
                }

                strTemp += "`" + m_arrFriends[i] + "`";
            }

            strTemp += ")";
        }
        else {
            strTemp += " (`" + GetName() + "`)";
        }

    }
    else {
        QString typeName = "";
        if (m_nType !=-1) {
            int nMax = sizeof (m_arrTypes) / sizeof (char*);

            if (m_nType >= nMax) {
                typeName = "[out of bound]";
            }
            else  {
                typeName = m_arrTypes[m_nType];
            }
        }

        if (GetArgs()!= "")  {
            strTemp = QString("%1 %2 %3").arg( GetName(), typeName, GetArgs());
        }
        else  {
            strTemp = QString("%1 %2").arg( GetName(), typeName);
        }

        if (m_bUnsigned) {
            strTemp +=  " UNSIGNED";
        }

        if (m_bNotNull) {
            strTemp += " NOT NULL";
        }

        if (m_bAuto) {
            strTemp += " auto_increment";
        }
    }

    return strTemp;
}

int CField::GetType()
{
    return m_nType;
}

void CField::SetType(int nType)
{
    m_nType = nType;
}

bool CField::IsUnique()
{
    return m_nType == FI_ALIAS_UNIQUE;
}

bool CField::IsPrimary()
{
    return m_nType == FI_ALIAS_PRIMARY;
}

bool CField::IsKey()
{
    return m_nType == FI_ALIAS_KEY;
}

bool CField::IsAlias()
{
    return m_nType >= FI_ALIAS_PRIMARY;
}

void CField::SetNotNull(bool bNotNull)
{
    m_bNotNull = bNotNull;
}

void CField::SetUnsigned(bool bUnsigned)
{
    m_bUnsigned = bUnsigned;
}

void CField::SetAuto(bool bAuto)
{
    m_bAuto = bAuto;
}

CTextArray & CField::GetFriends()
{
    return m_arrFriends;
}

QString CField::readString(FILE *sfile) const
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

void CField::writeString(FILE *tfile, const QString str)
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
