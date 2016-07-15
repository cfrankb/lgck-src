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
    m_functions = new CFunction [ m_fnMax];
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
    for (; i < m_fnCount - 1; ++i) {
        m_functions[i] = m_functions[i + 1];
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

        file << fn.name.trimmed();
        file << fn.desc.trimmed();
        file << fn.example.trimmed();

        file << fn.In().getSize();//.pCountIn;
        for (int j=0; j < fn.In().getSize(); ++j) {
            Param & p = fn.In()[j];
            file << p.name.trimmed();
            file << p.type.trimmed();
            file << p.desc.trimmed();
            file << p.flags;
        }

        file << fn.Out().getSize();//fn.pCountOut;
        for (int j=0; j < fn.Out().getSize(); ++j) {
            Param & p = fn.Out()[j];
            file << p.name.trimmed();
            file << p.type.trimmed();
            file << p.desc.trimmed();
            file << p.flags;
        }

        file << fn.state;
        file << fn.lang;
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

        file >> fn.name;
        file >> fn.desc;
        file >> fn.example;
        int size;
        file >> size;// fn.pCountIn;

        fn.In().forget();
        for (int j=0; j < size; ++j) {
//            Param & p = fn.paramsIn[j];
            Param p; // = fn.paramsIn[j];
            file >> p.name;
            file >> p.type;
            file >> p.desc;
            if (version > 1) {
                file >> p.flags;
            } else {
                p.flags = 0;
            }

            fn.In().add(p);
        }

        file >> size; //fn.pCountOut;
        fn.Out().forget();
        for (int j=0; j < size; ++j) {
//            Param & p = fn.paramsOut[j];
            Param p;// = fn.paramsOut[j];
            file >> p.name;
            file >> p.type;
            file >> p.desc;
            if (version > 1) {
                file >> p.flags;
            } else {
                p.flags = 0;
            }

            fn.Out().add(p);
        }

        if (version > 0) {
            file >> fn.state ;
            file >> fn.lang;
        } else {
            fn.state = 0;
            fn.lang = 0;
        }

        add(fn, false);
    }

    return true;
}

CFunctions & CFunctions::operator = (CFunctions & src)
{
    qDebug("eh\n");
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

        QString langs[] = {
            "n/a",
            "cpp",
            "lua"
        };

        char states[] = "?%@~";

        QString ret;
//        switch (fn.pCountOut) {
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
        hdr.sprintf("%c%s %s%s(", states[fn.state], q2c(ret), q2c(prefix), q2c(fn.name));

        QString paramsIn;
        QString expIn;
        bool hasExpIn = false;

        //fn.pCountIn
        for (int j = 0; j < fn.In().getSize(); ++j) {

            Param & param  = fn.In()[j];

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

            if (param.flags & CFunction::FLAG_OPTIONAL) {
                hdr += QString("[ %1 ]").arg(s);
            } else {
                hdr += s;
            }

            //fn.pCountIn
            if (j < fn.In().getSize() - 1) {
                hdr += ", ";
            }

            // IN

            if (!param.type.isEmpty() || !param.name.isEmpty()) {

                if (paramsIn.isEmpty()) {
                    paramsIn  = "  IN:  ";
                } else {
                    paramsIn += "       ";
                }

                paramsIn += QString ("[[ %1 ]] %2%3\n").arg(param.type).arg(param.name).arg(o);

                QString s;
                s.sprintf("   %-16s%s\n", q2c(param.name), q2c(param.desc));
                expIn += s;
                if (!param.desc.isEmpty()) {
                    hasExpIn = true;
                }
            }
        }

        hdr += QString(") [%1]\n\n").arg(langs[fn.lang]);

        if ( paramsIn.isEmpty() ) {
            paramsIn = "  IN:  na\n";
        }

        QString paramsOut;
        QString expOut;
        bool hasExpOut = false;

        //fn.pCountOut
        for (int j = 0; j < fn.Out().getSize(); ++j) {

            Param & param  = fn.Out()[j];

            if (!param.type.isEmpty() || !param.name.isEmpty()) {

                if (paramsOut.isEmpty()) {
                    paramsOut  = "  OUT: ";
                } else {
                    paramsOut += "       ";
                }

                paramsOut += QString ("[[ %1 ]] %2\n").arg(param.type).arg(param.name);

                QString s;
                s.sprintf("   %-16s%s\n", q2c(param.type), q2c(param.desc));
                expOut += s;
                if (!param.desc.isEmpty()) {
                    hasExpOut = true;
                }
            }
        }

        if ( paramsOut.isEmpty() ) {
            paramsOut = "  OUT: na\n";
        }

        file &= hdr;
        file &= paramsIn;
        file &= "\n";

        file &= paramsOut;
        //file += "\n";

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

        if (i != getSize() -1) {
            file &= "\n\n\n\n";
        }
    }

    file &= "\n";
}

CFunction & CFunction::operator = (CFunction & s)
{
    name = s.name;
    state = s.state;
    lang = s.lang;
    desc = s.desc;
    example = s.example;

    In().forget();
    Out().forget();

    for (int i=0; i < s.In().getSize(); ++i) {
        In().add(s.In()[i]);
    }

    for (int i=0; i < s.Out().getSize(); ++i) {
        Out().add(s.Out()[i]);
    }

    return *this;
}

void CFunction::copy (CFunction & s)
{
    *this = s;
}

void CFunctions::exportText(CFileWrap & file, QString prefix)
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
                //"?%@~";
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
        hdr.sprintf("@@function %s%s", q2c(prefix), q2c(fn.name));
        if (fn.lang) {
            hdr += " " + langs[fn.lang];
        }
        hdr += "\n";

        QString state = QString("@state %1\n").arg(states[fn.state]);

        QString paramsIn;
        for (int j = 0; j < fn.In().getSize(); ++j) {
            Param & param  = fn.In()[j];
            QString s = "";
            if (param.flags & CFunction::FLAG_OPTIONAL) {
                s += '*';
            }
            if (param.flags & CFunction::FLAG_MORE) {
                s += "...";
            }
            if (!param.type.isEmpty() || !param.name.isEmpty()) {
                paramsIn += QString("@in %1 %2%3\n").arg(param.type).arg(param.name).arg(s);
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
                paramsOut += QString("@out %1 %2%3\n").arg(param.type).arg(param.name).arg(s);
            }
        }
        file += hdr;
        file += state;
        file += paramsIn;
        file += paramsOut;
        if (!fn.desc.isEmpty()) {
            file += "@text\n\n";
            file += fn.desc;
            file += "\n";
        }
        file += "\n////////////////////////////////////////////////////////////////////////";
        if (i != getSize() -1) {
            file += "\n\n";
        }

    }
    file += "\n";
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
        for (int j = 0; j < fn.In().getSize(); ++j) {
            Param & param  = fn.In()[j];
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
            QString tmp = txt2wiki(fn.desc);
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
