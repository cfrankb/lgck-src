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
#include "Database.h"
#include "FileWrap.h"

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

    CFileWrap file;
    if (file.open(m_fileName)) {
        char buf[10];
        file.read( buf, 9 );
        if (memcmp(buf, "EASYDOC!", 8)==0) {
            file.seek(8);
            int version = VERSION;
            file >> version;
            if (version <= VERSION) {
                m_functions.read(file, version);
                m_classes.read(file, version);
                m_sections.read(file, version);
                result = true;
            } else {
                m_lastError = QString("unknown version %1").arg(version);
            }
        } else {
            if (memcmp(buf, "@@easydoc", 9)==0) {
                file.seek(0);
                //importText(file);
            } else {
                m_lastError = QString("wrong signature");
            }
        }
        file.close();

    } else {
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
    CFileWrap file;
    if (file.open(m_fileName, QIODevice::WriteOnly)) {
        file += m_signature;
        int version = VERSION;
        file << version;        
        m_functions.write(file);
        m_classes.write(file);
        m_sections.write(file);
        file.close();
    } else {
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

void CDatabase::dump(CFileWrap & file)
{
    bool header = true;
    if (header) {
        file += "<html>\r\n";
        file += " <head>\r\n";
    }
    file += "  <style type=\"text/css\">\r\n" \
            "   div.hdr0 {color:black; font-weight: bold; font-size: 16pt; border-bottom:thick dotted #000000; font-family:Verdana, Arial}\r\n"\
            "   div.hdr1 {color:black; font-weight: bold; font-size: 14pt; border-bottom: 1px solid; font-family:Verdana, Arial }\r\n"\
            "   div.hdr2 {color:black; font-weight: bold; font-size: 12pt; font-family:Verdana, Arial }\r\n"\
            "   div.hdr3 {color:black; font-weight: bold; font-style:italic; xborder-bottom:dotted; font-family:Verdana, Arial; xwidth:500px }\r\n"\
            "   div.fnUndef {cfont-size: 9pt; color:orange; font-weight: bold; border-top-width:thin; border-left-width:thin; border-style:solid; }\r\n"\
            "   div.fnTba {font-size: 9pt; color:brown; font-weight: bold; border-top-width:thin; border-left-width:thin; border-style:solid; width: 100%; }\r\n"\
            "   div.fnUntested {font-size: 9pt; color:gray; font-weight: bold; border-style:solid; border-top-width:thin; border-left-width:thin;}\r\n"\
            "   div.fnFinal {font-size: 9pt; color:blue; font-weight: bold; border-left-width:thin; border-top-width:thin; border-style:solid;  xbackground-color:yellow; xborder-color: black }\r\n"\
            "   div.code {border-left-width:thin; xmargin-top:1em; border-style:solid; border:2px dotted black;background:#eee; padding-left:20px; padding-bottom:20px; padding-top:10px; }\r\n"\
            "   pre.pageBody {width : 540px; font-size:12px;}\n" \
            "  </style>\r\n";
    if (header) {
        file += " </head>\r\n";

        file += " <body>\r\n" ;
    }
//    file += "  <pre style=\"width : 540px; font-size:12px;\">\r\n";
    file += "  <pre class=pageBody>\r\n";

    m_sections.dump(file);
    m_classes.dump(file);

    file &= "*** functions\n\n" ;
    m_functions.dump(file);

    file += "  </pre>\r\n";
    if (header) {
        file += " </body>\r\n</html>\r\n";
    }
}

void CDatabase::exportList(CFileWrap & file)
{
    m_classes.exportList(file);
    m_functions.exportList(file);
}

void CDatabase::exportText(CFileWrap & file)
{
    file += "@@easydoc\n\n";
    m_classes.exportText(file);
    m_functions.exportText(file);
}

void CDatabase::exportWiki(const QString & path)
{
    m_classes.exportWiki(path, &m_functions);
    CFileWrap file;

    file.open(path + "lua_functions.txt", QIODevice::WriteOnly);
    file += "===== Functions =====\n";
    m_functions.exportWiki(file, "");
    file.close();
}

/*
void CDatabase::importText(CFileWrap & file)
{
    const char edoc_head [] = "@@easydoc";

    enum {
        ED_NONE,
        ED_HEAD,
        ED_CLASS,
        ED_FUNCTION
    };

    int size = file.getSize();
    char *buf = new char[size+1];
    buf[size]=0;
    file.seek(0);
    file.read(buf,size);
    file.close();
    char *p = buf;
    int line = 1;
    int w = ED_NONE;
    while (*p) {
        char *n = strstr(p, '\n');
        char *r = strstr(p, '\r');
        char **pp = !n || !r ? std::max(n, r) : std::min(n, r);
        if (pp) {
            if (pp[0] == '\r' && pp[1] =='\n') {
                *pp = 0;
                ++pp;
            }
            *pp = 0;
            ++pp;
        }

        if (*p) {
            memcmp(p, edoc_head, strlen(edoc_head))
        }

        // next line
        ++line;
        p = pp;
    }

    delete [] buf;
}
*/
