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

#include <memory>
#include <cstring>
#include "Functions.h"
#include "Classes.h"
#include "helper.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/qtgui/qfilewrap.h"

CClasses::CClasses()
{
    m_cMax = GROWBY;
    m_cCount = 0;
    m_classes = new CClass *[m_cMax];

    for (int i = 0; i < m_cMax; ++i)
    {
        m_classes[i] = NULL;
    }
}

CClasses::~CClasses()
{
    if (m_classes)
    {
        for (int i = 0; i < m_cCount; ++i)
        {
            delete m_classes[i];
            m_classes[i] = NULL;
        }

        delete[] m_classes;
        m_classes = NULL;
    }
}

CClass *CClasses::operator[](int i)
{
    return m_classes[i];
}

int CClasses::add(CClass *cl, bool sorted)
{
    int i = 0;
    if (sorted && m_cCount)
    {
        // if this array is sorted, find where the new
        // node should be placed

        int min = 0;
        int max = m_cCount - 1;

        //  qDebug("count: %d", m_cCount);

        while (max >= min)
        {
            i = min + (max - min) / 2;
            //        qDebug("i=%d\n", i);
            if (cl->name().toLower() != m_classes[i]->name().toLower())
            {
                if (cl->name().toLower() > m_classes[i]->name().toLower())
                {
                    min = i + 1;
                }
                else
                {
                    max = i - 1;
                }
            }
            else
            {
                break;
            }
        }

        if (m_cCount && (cl->name().toLower() >= m_classes[i]->name().toLower()))
        {
            ++i;
        }
    }
    else
    {
        // if this array is unsorted simply append the new node
        // at the end

        i = m_cCount;
    }

    return insertAt(i, cl);
}

int CClasses::insertAt(int i, CClass *cl)
{
    //    qDebug("insertAt\n");
    if (m_cCount == m_cMax)
    {
        m_cMax += GROWBY;
        CClass **t = new CClass *[m_cMax];
        for (int j = 0; j < m_cCount; ++j)
        {
            t[j] = m_classes[j];
        }

        delete[] m_classes;
        m_classes = t;
    }

    for (int x = m_cCount; x > i; --x)
    {
        m_classes[x] = m_classes[x - 1];
    }

    m_classes[i] = cl;
    ++m_cCount;

    return i;
}

CClass *CClasses::removeAt(int i)
{
    CClass *t = m_classes[i];
    for (; i < m_cCount - 1; ++i)
    {
        m_classes[i] = m_classes[i + 1];
    }

    --m_cCount;
    m_classes[m_cCount] = NULL; // set extra record to null

    return t;
}

bool CClasses::read(QFileWrap &file, int version)
{
    m_cCount = 0;
    if (version >= 3)
    {
        int count;
        file >> count;

        for (int i = 0; i < count; ++i)
        {
            CClass *cl = new CClass;
            std::string name;
            file >> name;
            cl->name() = name.c_str();
            std::string desc;
            file >> desc;
            cl->desc() = desc.c_str();
            cl->methods().read(file, version);
            if (version >= 4)
            {
                cl->variables().read(file, version);
            }

            add(cl, true);
        }
    }

    return true;
}

bool CClasses::write(QFileWrap &file)
{
    file << m_cCount;
    for (int i = 0; i < m_cCount; ++i)
    {
        CClass *cl = m_classes[i];
        file << cl->name().toStdString();
        file << cl->desc().toStdString();
        cl->methods().write(file);
        // added v.0004
        cl->variables().write(file);
    }

    return true;
}

void CClasses::exportList(QFileWrap &file)
{
    for (int i = 0; i < m_cCount; ++i)
    {
        CClass *cl = m_classes[i];
        file += cl->name().toStdString();
        file += "\n";
        cl->methods().exportList(file, cl->name() + ":");
    }
}

void CClasses::removeAll()
{
    m_cCount = 0;
}

void CClasses::exportWiki(const QString &path, CFunctions *fct, QStringList &fileList)
{
    QFileWrap fileBin;
    fileBin.open(path + "lua_binding.txt", "w");
    fileList << "lua_binding.txt";

    fileBin += "===== Lua Binding =====\n";
    fileBin += "==== Classes ====\n";

    for (int i = 0; i < m_cCount; ++i)
    {
        CClass *cl = m_classes[i];
        QFileWrap file;
        QString fname = "class_" + cl->name().toLower();
        file.open(path + fname + ".txt", "w");
        fileList << fname + ".txt";
        fileBin += QString("  * [[%1|Class %2]]\n").arg(fname, cl->name()).toStdString();

        file += QString("===== Class %1 =====\n").arg(cl->name()).toStdString();
        if (!cl->desc().isEmpty())
        {
            file += "==== Description ====\n";
            auto t = new char[cl->desc().length() + 1024];
            file += (QString(txt2wiki(cl->desc(), t)) + "\n").toStdString();
            delete[] t;
        }

        file += "==== Members ====\n";
        for (int j = 0; j < cl->variables().getSize(); ++j)
        {
            Param &member = cl->variables()[j];
            file += QString("  * **" + member.type + "** " + member.name + "\\\\\n").toStdString();
        }
        if (cl->methods().getSize())
        {
            file += "\n\n";
        }
        cl->methods().exportWiki(file, cl->name() + ":");
        file.close();
    }

    fileBin += "==== Functions ====\n";
    fct->exportListWiki(fileBin, "");

    fileBin.close();
}
