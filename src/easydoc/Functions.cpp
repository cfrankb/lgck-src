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
#include "Functions.h"
#include "helper.h"
#include "../shared/qtgui/cheat.h"

CFunctions::CFunctions()
{
    m_fnMax = GROWBY;
    m_fnCount = 0;
    m_functions = new CFunction [m_fnMax];
}

CFunctions::~CFunctions()
{
    if (m_functions) {
        delete [] m_functions;
    }
}

CFunction & CFunctions::operator [] (int i)
{
    return m_functions[i];
}

int CFunctions::add(CFunction & fn, bool sorted)
{   
    int i = 0;

    if (sorted && m_fnCount) {
        // if this array is sorted, find where the new
        // node should be placed

        int min = 0;
        int max = m_fnCount - 1;

        while (max >= min) {
            i = min + (max - min) / 2;
            if (fn.name.toLower() != m_functions[i].name.toLower()) {
                if (fn.name.toLower() > m_functions[i].name.toLower())  {
                    min = i + 1;
                } else  {
                    max = i - 1;
                }
            } else  {
                break;
            }
        }

        if (m_fnCount && (fn.name.toLower() >= m_functions[i].name.toLower())) {
            ++i;
        }
    } else {
        // if this array is unsorted simply append the new node
        // at the end

        i = m_fnCount;
    }

    return insertAt(i, fn);
}

int CFunctions::insertAt(int i, CFunction & fn)
{
    if (m_fnCount == m_fnMax) {
        m_fnMax += GROWBY;
        CFunction *t  = new CFunction[m_fnMax];
        for (int i=0; i < m_fnCount; ++i) {
            t [ i ] = m_functions [ i ];
        }
        delete [] m_functions;
        m_functions = t;
    }

    for (int x = m_fnCount; x > i; --x) {
        m_functions[x] = m_functions[x - 1];
    }

    m_functions[ i ] = fn;
    ++m_fnCount;

    return i;
}

void CFunctions::removeAt(int i)
{
    for (int j=i; j < m_fnCount - 1; ++j) {
        m_functions[j] = m_functions[j + 1];
    }
    --m_fnCount;
}

void CFunctions::removeAll()
{
    m_fnCount = 0;
}

bool CFunctions::write(CFileWrap & file)
{
    file << m_fnCount;
    for (int i=0; i < m_fnCount; ++i) {
        CFunction & fn = m_functions[i];
        fn.write(file);
    }
    return true;
}

bool CFunctions::read(CFileWrap & file, int version)
{
    removeAll();
    int count;
    file >> count;
    for (int i=0; i < count; ++i) {
        CFunction fn;
        fn.read(file, version);
        add(fn, false);
    }
    return true;
}

CFunctions & CFunctions::operator = (CFunctions & src)
{
    m_fnCount = 0;
    for (int i = 0; i < src.getSize(); ++i) {
        add(src[i], false);
    }
    return *this;
}

void CFunctions::exportList(CFileWrap & file, QString prefix)
{
    for (int i = 0; i < getSize(); ++i) {
        file += prefix;
        file += m_functions[i].name;
        file += "\n";
    }
}

void CFunctions::dump(CFileWrap & file, QString prefix)
{
    for (int i = 0; i < getSize(); ++i) {
        CFunction & fn = m_functions[i];
        char states[] = "?%@~";
        QString ret;
        switch (fn.Out().getSize()) {
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
        hdr.sprintf("%c %s%s()\n", states[fn.state], q2c(prefix), q2c(fn.name));
        QString paramsIn = "";
        QString paramsInAll = "";
        QString expIn;
        bool hasExpIn = false;

        for (int k=0; k<fn.InSetCount(); ++k) {
            paramsIn = "";
            for (int j = 0; j < fn.In(k).getSize(); ++j) {
                Param & param  = fn.In(k)[j];
                char o[2];
                o[0] = 0;
                o[1] = 0;
                if (param.flags & CFunction::FLAG_OPTIONAL) {
                    o[0] = '*';
                }

                QString s;
                if (param.name.isEmpty()) {
                    s = param.type;
                } else {
                    s = QString("%1 %2").arg(param.type).arg(param.name);
                }

                if (param.flags & CFunction::FLAG_MORE) {
                    s += "...";
                }

                // IN

                if (!param.type.isEmpty() || !param.name.isEmpty()) {
                    if (paramsIn.isEmpty()) {
                        paramsIn  = "  IN:  ";
                    } else {
                        paramsIn += "       ";
                    }
                    paramsIn += QString ("<span class=\"typeany\">%1</span> %2%3\n").arg(param.type).arg(param.name).arg(o);
                    QString s;
                    s.sprintf("   %-16s%s\n", q2c(param.name), q2c(param.desc));
                    expIn += s;
                    if (!param.desc.isEmpty()) {
                        hasExpIn = true;
                    }
                }
            }
            if (!paramsInAll.isEmpty()) {
                paramsInAll += "\n";
            }
            paramsInAll += paramsIn;
        }

        if ( paramsInAll.isEmpty() ) {
            paramsInAll = "  IN:  void\n";
        }

        QString paramsOut;
        QString expOut;
        bool hasExpOut = false;
        for (int j = 0; j < fn.Out().getSize(); ++j) {
            Param & param  = fn.Out()[j];
            if (!param.type.isEmpty() || !param.name.isEmpty()) {
                if (paramsOut.isEmpty()) {
                    paramsOut  = "  OUT: ";
                } else {
                    paramsOut += "       ";
                }
                paramsOut += QString ("<span class=\"typeany\">%1</span> %2\n").arg(param.type).arg(param.name);
                QString s;
                s.sprintf("   %-16s%s\n", q2c(param.type), q2c(param.desc));
                expOut += s;
                if (!param.desc.isEmpty()) {
                    hasExpOut = true;
                }
            }
        }

        if ( paramsOut.isEmpty() ) {
            paramsOut = "  OUT: void\n";
        }

        file += QString("<a name=\"%1%2\"></a>").arg(prefix).arg(fn.name);
        file &= hdr;
        file &= paramsInAll;
        file &= "\n";
        file &= paramsOut;

        if (hasExpIn) {
            file &= "\n\n";
            file &= "+++paramIns:\n\n";
            file &= expIn;
            file &= "\n";
        }

        if (hasExpOut) {
            file &= "\n\n";
            file &= "+++paramOuts:\n\n";
            file &= expOut;
            file &= "\n";
        }

        if (!fn.desc.isEmpty()) {
            file &= "\n";
            file &= "+++description:\n\n";
            file &= fn.desc;
        }

        if (fn.m_alias.count()) {
            file &= "\n+++alias:\n";
            QListIterator<QString> itr (fn.m_alias);
            while (itr.hasNext()) {
                QString alias = itr.next();
                if (find(alias)!=-1) {
                    file += QString("&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"#%1%2\">%1%2</a><br>\n").arg(prefix).arg(alias);
                } else {
                    file += QString("&nbsp;&nbsp;&nbsp;&nbsp;%1<br>\n").arg(alias);
                }
            }
            file &= "\n";
        }

        if (i != getSize() -1) {
            file &= "\n\n\n\n";
        }

    }
    file &= "\n";
}

void CFunction::copy(CFunction & s)
{
    *this = s;
}

void CFunctions::exportWiki(CFileWrap & file, QString prefix)
{
    for (int i = 0; i < getSize(); ++i) {
        CFunction & fn = m_functions[i];
        QString langs[] = {
            "n/a",
            "cpp",
            "lua"
        };
        const char *states[] =  {
            "?",
            "finished",
            "tba",
            "untested"
        };

        QString ret;
        switch (fn.Out().getSize()) {
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
        QString hdr;
        QString paramsIn;
        for (int k=0; k<fn.InSetCount(); ++k) {
            for (int j = 0; j < fn.In(k).getSize(); ++j) {
                Param & param  = fn.In(k)[j];
                QString s = "";
                if (param.flags & CFunction::FLAG_OPTIONAL) {
                    s += '*';
                }
                if (param.flags & CFunction::FLAG_MORE) {
                    s += "...";
                }
                if (!param.type.isEmpty() || !param.name.isEmpty()) {
                    paramsIn += QString("  * __in__ **%1 %2**%3\\\\\n").arg(param.type).arg(param.name).arg(s);
                }
            }
            if (paramsIn.isEmpty()) {
                paramsIn = "  * __in__ ** void **\\\\\n";
            }
        }

        QString paramsOut;
        for (int j = 0; j < fn.Out().getSize(); ++j) {
            Param & param  = fn.Out()[j];
            QString s = "";
            if (param.flags & CFunction::FLAG_OPTIONAL) {
                s += '*';
            }
            if (param.flags & CFunction::FLAG_MORE) {
                s += "...";
            }
            if (!param.type.isEmpty() || !param.name.isEmpty()) {
                paramsOut += QString("  * __out__ **%1 %2**%3\\\\\n").arg(param.type).arg(param.name).arg(s);
            }
        }
        if (paramsOut.isEmpty()) {
            paramsOut = "  * __out__ ** void **\\\\\n";
        }

        hdr.sprintf("==== %s%s", q2c(prefix), q2c(fn.name));
        if (state != "finished") {
            if (fn.lang) {
                hdr += " (" + langs[fn.lang] + "/" + state + ")";
            } else {
                hdr += " (" + state + ")";
            }
        }
        hdr += "====\n";
        file += hdr;
//      file += state;
        file += paramsIn;
        file += paramsOut;
        if (!fn.desc.isEmpty()) {
            file += "=== Description ===\n\n";
            char t[fn.desc.length()+1024];
            QString tmp = txt2wiki(fn.desc, t);
            file += tmp;
            file += "\n";
        }
        if (i != getSize() -1) {
            file += "\n\n";
        }
        file += "\n\n";
    }
    file += "\n";
}

void CFunctions::exportListWiki(CFileWrap & file, QString prefix)
{
    for (int i = 0; i < getSize(); ++i) {
        CFunction & fn = m_functions[i];
        QString langs[] = {
            "n/a",
            "cpp",
            "lua"
        };
        const char *states[] =  {
            "?",
            "finished",
            "tba",
            "untested"
        };
        QString ret;
        switch (fn.Out().getSize()) {
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
        QString hdr;
        hdr.sprintf("  * [[Lua_Functions#%s%s|%s%s", q2c(prefix), q2c(fn.name),
                    q2c(prefix), q2c(fn.name));
        if (state != "finished") {
            if (fn.lang) {
                hdr += " (" + langs[fn.lang] + "/" + state + ")";
            } else {
                hdr += " (" + state + ")";
            }
        }
        hdr += "]]\n";
        file += hdr;
        if (i != getSize() -1) {
            //file += "\n\n";
        }
    }
    file += "\n";
}

static CFunction fnTmp;
CFunction &CFunctions::operator [](const QString & name)
{
    int i = find(name);
    if (i != -1) {
        return m_functions[i];
    } else {
        return fnTmp;
    }
}

int CFunctions::find(const QString & name)
{
    for (int i = 0; i < getSize(); ++i) {
        if (m_functions[i].name==name) {
            return i;
        }
    }
    return -1;
}

void CFunction::read(CFileWrap & file, int version)
{
    file >> name;
    file >> desc;
    file >> example;
    if (version > 5) {
        m_inCount = 0;
        file.read(&m_inCount,1);
        for (int i=0; i<m_inCount; ++i) {
            In(i).read(file, version);
        }
    } else {
        In(0).read(file, version);
        m_inCount = 1;
    }
    Out().read(file, version);
    if (version > 0) {
        file >> state;
        file >> lang;
    } else {
        state = 0;
        lang = 0;
    }
    if (version > 4) {
        file >> m_alias;
    } else {
        m_alias.clear();
    }
}

void CFunction::write(CFileWrap & file)
{
    file << name.trimmed();
    file << desc.trimmed();
    file << example.trimmed();
    file.write(&m_inCount,1);
    for (int i = 0; i < m_inCount; ++i) {
        In(i).write(file);
    }
    Out().write(file);
    file << state;
    file << lang;
    file << m_alias;
}

void CFunction::init()
{
    m_inCount = 1;
    for (int i=0; i < MAX_IN_COUNT; ++i) {
        In(i).forget();
    }
    Out().forget();
    state = 0;
    lang = 0;
}

void CFunction::removeInSet(int i)
{
    for (int j=i; j < m_inCount-1; ++j) {
        paramsIn[i] =paramsIn[i+1];
    }
    --m_inCount;
}
