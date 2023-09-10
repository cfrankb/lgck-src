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
#include "Functions.h"
#include "helper.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/qtgui/qfilewrap.h"

CFunctions::CFunctions()
{
    m_fnMax = GROWBY;
    m_fnCount = 0;
    m_functions = new CFunction[m_fnMax];
}

CFunctions::~CFunctions()
{
    if (m_functions)
    {
        delete[] m_functions;
    }
}

CFunction &CFunctions::operator[](int i)
{
    return m_functions[i];
}

int CFunctions::add(CFunction &fn, bool sorted)
{
    int i = 0;

    if (sorted && m_fnCount)
    {
        // if this array is sorted, find where the new
        // node should be placed

        int min = 0;
        int max = m_fnCount - 1;

        while (max >= min)
        {
            i = min + (max - min) / 2;
            if (fn.name.toLower() != m_functions[i].name.toLower())
            {
                if (fn.name.toLower() > m_functions[i].name.toLower())
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

        if (m_fnCount && (fn.name.toLower() >= m_functions[i].name.toLower()))
        {
            ++i;
        }
    }
    else
    {
        // if this array is unsorted simply append the new node
        // at the end

        i = m_fnCount;
    }

    return insertAt(i, fn);
}

int CFunctions::insertAt(int i, CFunction &fn)
{
    if (m_fnCount == m_fnMax)
    {
        m_fnMax += GROWBY;
        CFunction *t = new CFunction[m_fnMax];
        for (int i = 0; i < m_fnCount; ++i)
        {
            t[i] = m_functions[i];
        }
        delete[] m_functions;
        m_functions = t;
    }

    for (int x = m_fnCount; x > i; --x)
    {
        m_functions[x] = m_functions[x - 1];
    }

    m_functions[i] = fn;
    ++m_fnCount;

    return i;
}

void CFunctions::removeAt(int i)
{
    for (int j = i; j < m_fnCount - 1; ++j)
    {
        m_functions[j] = m_functions[j + 1];
    }
    --m_fnCount;
}

void CFunctions::removeAll()
{
    m_fnCount = 0;
}

bool CFunctions::write(QFileWrap &file)
{
    file << m_fnCount;
    for (int i = 0; i < m_fnCount; ++i)
    {
        CFunction &fn = m_functions[i];
        fn.write(file);
    }
    return true;
}

bool CFunctions::read(QFileWrap &file, int version)
{
    removeAll();
    int count;
    file >> count;
    for (int i = 0; i < count; ++i)
    {
        CFunction fn;
        fn.read(file, version);
        add(fn, false);
    }
    return true;
}

CFunctions &CFunctions::operator=(CFunctions &src)
{
    m_fnCount = 0;
    for (int i = 0; i < src.getSize(); ++i)
    {
        add(src[i], false);
    }
    return *this;
}

void CFunctions::exportList(QFileWrap &file, QString prefix)
{
    for (int i = 0; i < getSize(); ++i)
    {
        file += prefix.toStdString();
        file += m_functions[i].name.toStdString();
        file += "\n";
    }
}


void CFunction::copy(CFunction &s)
{
    *this = s;
}

void CFunctions::exportWiki(QFileWrap &file, QString prefix)
{
    for (int i = 0; i < getSize(); ++i)
    {
        CFunction &fn = m_functions[i];

        QString ret;
        switch (fn.Out().getSize())
        {
        case 0:
            ret = "void";
            break;

        case 1:
            ret = fn.Out()[0].type;
            break;

        default:
            ret = "mixed";
            break;
        }

        QString hdr;
        QString paramsIn;
        QStringList optional;
        for (int k = 0; k < fn.InSetCount(); ++k)
        {
            for (int j = 0; j < fn.In(k).getSize(); ++j)
            {
                Param &param = fn.In(k)[j];
                QString s = "";
                if (param.flags & CFunction::FLAG_OPTIONAL)
                {
                    s += '*';
                    optional << QString("in parameter ** %1 ** : optional and can be omitted.\n").arg(param.name);
                }
                if (param.flags & CFunction::FLAG_MORE)
                {
                    s += "...";
                }
                if (!param.type.isEmpty() || !param.name.isEmpty())
                {
                    paramsIn += QString("  * __in__ **%1 %2**%3\\\\\n").arg(param.type).arg(param.name).arg(s);
                }
            }
            if (paramsIn.isEmpty())
            {
                paramsIn = "  * __in__ ** void **\\\\\n";
            }
        }

        QString paramsOut;
        for (int j = 0; j < fn.Out().getSize(); ++j)
        {
            Param &param = fn.Out()[j];
            QString s = "";
            if (param.flags & CFunction::FLAG_OPTIONAL)
            {
                s += '*';
            }
            if (param.flags & CFunction::FLAG_MORE)
            {
                s += "...";
            }
            if (!param.type.isEmpty() || !param.name.isEmpty())
            {
                paramsOut += QString("  * __out__ **%1 %2**%3\\\\\n").arg(param.type).arg(param.name).arg(s);
            }
        }
        if (paramsOut.isEmpty())
        {
            paramsOut = "  * __out__ ** void **\\\\\n";
        }

        hdr = QString::asprintf("===== %s%s =====\n", q2c(prefix), q2c(fn.name));
        file += hdr.toStdString();
        file += paramsIn.toStdString();
        file += paramsOut.toStdString();
        QString tmp = "";
        if (!fn.desc.isEmpty())
        {
            auto t = new char[fn.desc.length() + 1024];
            tmp = txt2wiki(fn.desc, t);
            delete[] t;
        }
        if (optional.length())
        {
            if (!tmp.isEmpty())
            {
                tmp += "\n\n";
            }
            tmp += optional.join('\n');
        }
        if (!tmp.isEmpty())
        {
            file += tmp.toStdString();
            file += "\n";
        }

        QString text;
        if (fn.m_alias.length())
        {
            file += "\n** Alias: **";
            foreach (text, fn.m_alias)
            {
                text = text.trimmed();
                QString tmp;
                if (text.contains(":"))
                {
                    QStringList list = text.toLower().split(":");
                    tmp = QString("class_%1#%2").arg(list[0]).arg(list[1]);
                }
                else
                {
                    tmp = QString("lua_functions#%1").arg(text);
                }
                file += QString("%1 ").arg(text).toStdString();
            }
            file += "\n\n";
        }

        if (i != getSize() - 1)
        {
            file += "----\n\n";
        }
        file += "\n\n";
    }
    file += "\n";
}

void CFunctions::exportListWiki(QFileWrap &file, QString prefix)
{
    for (int i = 0; i < getSize(); ++i)
    {
        CFunction &fn = m_functions[i];
        QString langs[] = {
            "n/a",
            "cpp",
            "lua"};
        const char *states[] = {
            "?",
            "finished",
            "tba",
            "untested"};
        QString ret;
        switch (fn.Out().getSize())
        {
        case 0:
            ret = "void";
            break;

        case 1:
            ret = fn.Out()[0].type;
            break;

        default:
            ret = "mixed";
            break;
        }
        QString state = QString(states[fn.state]);
        QString hdr = QString::asprintf("  * [[Lua_Functions#%s%s|%s%s", q2c(prefix), q2c(fn.name),
                                        q2c(prefix), q2c(fn.name));

        if (state != "finished")
        {
            if (fn.lang)
            {
                hdr += " (" + langs[fn.lang] + "/" + state + ")";
            }
            else
            {
                hdr += " (" + state + ")";
            }
        }
        hdr += "]]\n";
        file += hdr.toStdString();
        if (i != getSize() - 1)
        {
            // file += "\n\n";
        }
    }
    file += "\n";
}

static CFunction fnTmp;
CFunction &CFunctions::operator[](const QString &name)
{
    int i = find(name);
    if (i != -1)
    {
        return m_functions[i];
    }
    else
    {
        return fnTmp;
    }
}

int CFunctions::find(const QString &name)
{
    for (int i = 0; i < getSize(); ++i)
    {
        if (m_functions[i].name == name)
        {
            return i;
        }
    }
    return -1;
}

void CFunction::read(QFileWrap &file, int version)
{
    std::string tmp;
    file >> tmp;
    name = tmp.c_str();
    file >> tmp;
    desc = tmp.c_str();
    file >> tmp;
    example = tmp.c_str();
    if (version > 5)
    {
        m_inCount = 0;
        file.read(&m_inCount, 1);
        for (int i = 0; i < m_inCount; ++i)
        {
            In(i).read(file, version);
        }
    }
    else
    {
        In(0).read(file, version);
        m_inCount = 1;
    }
    Out().read(file, version);
    if (version > 0)
    {
        file >> state;
        file >> lang;
    }
    else
    {
        state = 0;
        lang = 0;
    }
    if (version > 4)
    {
        /////////////////////////////////
        // write StringList
        int size;
        file.read(&size, sizeof(size));
        for (int i = 0; i < size; ++i)
        {
            std::string tmp;
            file >> tmp;
            m_alias.append(tmp.c_str());
        }
        /////////////////// file >> m_alias;
    }
    else
    {
        m_alias.clear();
    }
    if (version > 6)
    {
        file.read(&m_testCaseCount, sizeof(m_testCaseCount));
        for (int i = 0; i < m_testCaseCount; ++i)
        {
            testcase[i].read(file, version);
        }
    }
    else
    {
        m_testCaseCount = 0;
    }
}

void CFunction::write(QFileWrap &file)
{
    file << name.trimmed().toStdString();
    file << desc.trimmed().toStdString();
    file << example.trimmed().toStdString();
    file.write(&m_inCount, 1);
    for (int i = 0; i < m_inCount; ++i)
    {
        In(i).write(file);
    }
    Out().write(file);
    file << state;
    file << lang;
    ////////////////////////////////
    //file << m_alias;
    int size = m_alias.size();
    file.write(&size, sizeof(size));
    QListIterator<QString> itr(m_alias);
    while (itr.hasNext())
    {
        QString current = itr.next();
        file << current.toStdString();
    }
    ///////////////////////////////

    bool go = true;
    if (go)
    {
        file.write(&m_testCaseCount, sizeof(m_testCaseCount));
        for (int i = 0; i < m_testCaseCount; ++i)
        {
            testcase[i].write(file);
        }
    }
}

void CFunction::init()
{
    m_inCount = 1;
    for (int i = 0; i < MAX_IN_COUNT; ++i)
    {
        In(i).forget();
    }
    Out().forget();
    state = 0;
    lang = 0;
    m_testCaseCount = 0;
}

void CFunction::removeInSet(int i)
{
    for (int j = i; j < m_inCount - 1; ++j)
    {
        paramsIn[j] = paramsIn[j + 1];
    }
    --m_inCount;
}

void CFunction::removeTestCase(int i)
{
    for (int j = i; j < m_testCaseCount - 1; ++j)
    {
        testcase[j] = testcase[j + 1];
    }
    --m_testCaseCount;
}
