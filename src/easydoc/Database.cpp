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

#include "Database.h"
#include "../shared/qtgui/qfilewrap.h"

CDatabase::CDatabase()
{
    m_dirty = false;
}

CDatabase::~CDatabase()
{
}

char CDatabase::m_signature[] = "EASYDOC!";

bool CDatabase::read()
{
    bool result = false;

    QFileWrap file;
    if (file.open(m_fileName))
    {
        char buf[10];
        file.read(buf, 9);
        if (memcmp(buf, "EASYDOC!", 8) == 0)
        {
            file.seek(8);
            int version = 0;
            file >> version;
            if (version <= getVersion())
            {
                m_functions.read(file, version);
                m_classes.read(file, version);
                m_sections.read(file, version);
                result = true;
            }
            else
            {
                m_lastError = QString("unknown version: %1").arg(version);
            }
        }
        else
        {
            m_lastError = QString("wrong signature");
        }
        file.close();
    }
    else
    {
        m_lastError = QString("can't read %1").arg(m_fileName);
    }
    return result;
}

void CDatabase::removeAll()
{
    m_functions.removeAll();
    m_classes.removeAll();
    m_sections.removeAll();
}

bool CDatabase::write()
{
    QFileWrap file;
    if (file.open(m_fileName, "w"))
    {
        file += m_signature;
        int version = getVersion();
        file << version;
        m_functions.write(file);
        m_classes.write(file);
        m_sections.write(file);
        file.close();
    }
    else
    {
        m_lastError = QString("can't write %1").arg(m_fileName);
        return false;
    }
    return true;
}

void CDatabase::setFileName(const QString fileName)
{
    m_fileName = fileName;
}

QString CDatabase::getFileName()
{
    return m_fileName;
}


void CDatabase::exportList(QFileWrap &file)
{
    m_classes.exportList(file);
    m_functions.exportList(file);
}

void CDatabase::exportWiki(const QString &path, QStringList &fileList)
{
    fileList = QStringList();
    fileList += "lua_functions.txt";
    m_classes.exportWiki(path, &m_functions, fileList);
    QFileWrap file;
    file.open(path + "lua_functions.txt", "w");
    file += "====== Functions ======\n";
    m_functions.exportWiki(file, "");
    file.close();
    file.open(path + "remotesync.dat", "w");
    QString strList = fileList.join("\n");
    file.write(strList.toLatin1().data(), strList.length());
    file.close();
}

void CDatabase::importGameLua(const char *cdata)
{
    char *data = new char[strlen(cdata) + 1];
    strcpy(data, cdata);
    char *p = data;
    while (*p)
    {
        char *pe = strstr(p, "\n");
        if (pe)
        {
            *pe = 0;
        }
        char *t = strstr(p, "alias");
        if (t && *p != '#')
        {
            char *pp = strstr(t, "\"");
            char *alias_name = NULL;
            char *fn_name = NULL;
            if (pp)
            {
                t = strstr(++pp, "\"");
                if (t)
                {
                    *t = 0;
                    alias_name = pp;
                    t = strstr(++t, ",");
                    if (t)
                    {
                        ++t;
                        while (*t == ' ')
                            ++t;
                        fn_name = t;
                        while (isalnum(*t) || *t == '_')
                            ++t;
                        *t = 0;
                    }
                }
            }
            if (alias_name && fn_name)
            {
                int iAlias = m_functions.find(alias_name);
                int iFn = m_functions.find(fn_name);
                if (iAlias != -1 && iFn == -1)
                {
                    qDebug("alias %s >> %s", alias_name, fn_name);
                    CFunction fn;
                    fn.copy(m_functions[alias_name]);
                    m_functions.removeAt(iAlias);
                    // add function (renamed)
                    fn.name = fn_name;
                    fn.m_alias.append(alias_name);
                    m_functions.add(fn, true);
                    // add alias
                    /*
                    fn.m_alias.clear();
                    fn.desc = QString("This is an alias for %1.").arg(fn_name);
                    fn.example = "";
                    m_functions.add(fn, true);*/
                }
                else if (iFn != -1)
                {
                    qDebug("fn %s add alias %s", fn_name, alias_name);
                    CFunction &fn = m_functions[fn_name];
                    if (!fn.m_alias.contains(alias_name))
                    {
                        fn.m_alias.append(alias_name);
                    }
                }
                else
                {
                    qDebug("EXCEPTION fn %s add alias %s", fn_name, alias_name);
                }
            }
        }
        // next line
        if (pe)
        {
            p = ++pe;
        }
    }
    delete[] data;
}

int CDatabase::getVersion()
{
    return 0x0007;
}
