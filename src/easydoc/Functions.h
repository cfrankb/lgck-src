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


#ifndef FUNCTIONS_H
#define FUNCTIONS_H

class CFileWrap;
#include "Params.h"

class CFunction
{
public:
    CFunction() {}
    /*CFunction(CFunction & s) {
        name = s.name;
        state = s.state;
        lang = s.lang;
        desc = s.desc;
        example = s.example;
        m_alias = s.m_alias;
        paramsIn = s.In();
        paramsOut = s.Out();
    }*/
    ~CFunction() {}

    //CFunction & operator = (CFunction & s);
    void copy(CFunction & s);
    CParams & In() { return paramsIn; }
    CParams & Out()  { return paramsOut; }
    void read(CFileWrap & file, int version);
    void write(CFileWrap & file);

    QString name;
    int state;
    int lang;
    QString desc;
    QString example;
    QStringList m_alias;

    enum {
        FLAG_OPTIONAL = 1,
        FLAG_MORE     = 2
    };

protected:
    CParams paramsIn;
    CParams paramsOut;
};

class CFunctions
{

public:
    CFunctions();
    ~CFunctions();

    CFunction & operator [] (int i);
    CFunction & operator [] (const QString & name);
    CFunctions & operator = (CFunctions & src);
    int getSize() const {
        return m_fnCount;
    }

    int find(const QString & name);
    int add(CFunction & fn, bool sorted);
    int insertAt(int i, CFunction & fn);
    void removeAt(int i);
    void removeAll();

    bool write(CFileWrap & file);
    bool read(CFileWrap & file, int version);
    void dump(CFileWrap & file, QString prefix="");
    void exportList(CFileWrap & file, QString prefix="");
    void exportText(CFileWrap & file, QString prefix="");
    void exportWiki(CFileWrap & file, QString prefix="");
    void exportListWiki(CFileWrap & file, QString prefix);

protected:
    enum {
        GROWBY = 16
    };

    CFunction *m_functions;
    int m_fnCount;
    int m_fnMax;
};

#endif // FUNCTIONS_H
