/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2011  Francois Blanchette

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
#include "Settings.h"
#include "../shared/IFile.h"
#include <cstring>
#include <cstdlib>

CSettings::CSettings()
{
    m_settings = nullptr;
    m_bReplaceAll = false;
}

CSettings::~CSettings()
{
    if (m_settings) {
        delete [] m_settings;
    }
}

void CSettings::copySettings(const SETTING * defaults)
{
    int count;
    for (count = 0; defaults[count].param.c_str()[0]; ++count) ;
    if (m_settings){
        delete [] m_settings;
    }
    m_settings = new SETTING [ count + 1];
    for (int i=0; i< count; ++i) {
        (m_settings)[i] = defaults[i];
    }
    (m_settings)[count].param = "";
}

bool CSettings::parseSettings(const char *strText)
{
    // if we're replacing everything, remove all param entries
    if (m_bReplaceAll) {
        removeAll();
    }

    char *buffer = new char [ strlen(strText) + 1];
    strcpy( buffer, strText );
    char *org = buffer;

    char d[2];
    d[1] = 0;
    if (strstr(buffer, "\r")) {
        d[0] = '\r';
    } else {
        d[0] = '\n';
    }

    do {
        char *next = strstr(buffer, d);
        if (next) {
            *next =0;
            next ++;
        }

        while ( (*buffer == '\n') || (*buffer == ' ') || (*buffer == '\t')) {
            buffer++;
        }

        if (*buffer != '#' && *buffer != ';') {
            char *p = strstr(buffer, "\t");
            while (p) {
                *p = ' ';
                p = strstr(buffer, "\t");
            }
            p = buffer + strlen(buffer) - 1;
            while ( (p > buffer) && (*p == ' ')) {
                *p = 0;
                p--;
            }

            std::string param;
            std::string tmp;
            std::string value;
            int valueInt = 0;

            p = buffer;
            while (*p) {
                if (*p == ' ') {
                    *p = 0;
                    ++p;
                    while (*p == ' ') ++p;
                    if (*p == '~') ++p;
                    tmp = p;
                    removeSlashes(tmp, value);
                    while (*p && isdigit(*p)) {
                        valueInt = 10 *valueInt + *p - '0';
                        ++p;
                    }
                    break;
                }

                if (*p == '\\') {
                    ++p;
                }

                if (*p) {
                    ++p;
                }
            }

            param = buffer;
            if (!param.empty()) {
                if (m_bReplaceAll) {
                    add(param.c_str(), value.c_str());
                } else {
                    int i;
                    for (i=0; m_settings[i].param.c_str()[0]; i++) {
                        if (m_settings[i].param == param) {
                            m_settings[i].value = value;
                            m_settings[i].valueInt = valueInt;
                            break;
                        }
                    }
                }
            }
        }

        buffer = next;
    } while (buffer && *buffer);

    delete [] org;
    return true;
}

int CSettings::calculateCost(const std::string & s)
{
    int cost = s.length();
    if (s[0]==' ') {
        ++cost;
    }
    for (unsigned int i=0; i < s.length(); ++i) {
        if (s[i]=='\\') {
            ++cost;
        }
    }
    return cost;
}

void CSettings::outputSettings(std::string &t)
{
    if (m_settings) {
        int bufSize = 1024;
        int curSize = 0;
        char *buf = new char[bufSize];
        buf[0]=0;
        for (int i=0; !m_settings[i].param.empty(); ++i) {
            if (m_settings[i].param[0] != '#') {
                int addIn = strlen(m_settings[i].param.c_str()) + 2 + calculateCost(m_settings[i].value);
                if (curSize + addIn + 1 >= bufSize) {
                    bufSize += 2048 + addIn;
                    char *p = new char[bufSize];
                    strcpy(p, buf);
                    delete [] buf;
                    buf = p;
                }
                strcat(buf, m_settings[i].param.c_str());
                strcat(buf, " ");
                addSlashes2(m_settings[i].value, buf + strlen(buf));
                strcat(buf, "\n");
                curSize += addIn;
            }
        }
        t = buf;
        delete [] buf;
    } else {
        t = "# Bad: source array is NULL\n";
    }
}

void CSettings::addSlashes2(const std::string & s, char *t)
{
    for (unsigned int i=0; i < s.length(); ++i) {
        if (i==0 && s[i]==' ') {
            *t = '~';
            ++t;
            *t = ' ';
            ++t;
            continue;
        }

        switch (s[i]) {
        case '\\':
            *t = '\\';
            ++t;
            *t = '\\';
        break;

        case '\n':
            *t = '\\';
            ++t;
            *t = 'n';
            break;

        case '\r':
            *t = '\\';
            ++t;
            *t = 'r';
            break;

        default:
            *t = s[i];
        }
        ++t;
    }
    *t=0;
}

void CSettings::removeSlashes(const std::string & s, std::string & t)
{
    // TODO: check this function again
    t = "";
    for (unsigned int i=0; i < s.length(); ++i) {
        if (s[i] == '\\') {
            ++i;
            if (s[i] == 'n') {
                t += '\n';
            } else if (s[i] == 'r') {
                t += '\r';
            } else {
                t += s[i];
            }
        } else {
            t += s[i];
        }
    }
}


CSettings & CSettings::operator >> (IFile &file)
{
    std::string t;
    outputSettings(t);
    file << t;//.c_str();
    return *this;
}

CSettings & CSettings::operator << (IFile &file)
{
    std::string str;
    file >> str;
    parseSettings(str.c_str());
    return *this;
}

CSettings::SETTING & CSettings::operator [] (const char *param)
{
    int i;
    for (i=0; m_settings[i].param.c_str()[0]; i++) {
        if (! m_settings[i].param.compare(param)) {
            return m_settings[i];
        }
    }
    return m_settings[i]; // return dummy placeholder
}

int CSettings::getSize()
{
    int i;
    for (i=0; m_settings[i].param.c_str()[0]; i++);

    return i;
}

CSettings & CSettings::operator = (CSettings & s)
{
    if (m_settings) {
        delete [] m_settings;
        m_settings = nullptr;
    }

    int size = s.getSize();
    m_settings = new SETTING [ size + 1];
    for (int i = 0; i < size; ++i) {
        m_settings [i] = s[i];
    }

    return *this;
}

void CSettings::add(const char *param, const char *value)
{
    if (!m_settings) {
        m_settings = new SETTING[1];
    }

    int size = getSize();
    SETTING *t = new SETTING [ size + 2];
    for (int i = 0; i < size ; ++i) {
        t[i] = m_settings[i];
    }

    t[size].param = param;
    t[size].value = value;
    t[size].valueInt = strtol(value, nullptr, 10);

    delete [] m_settings;
    m_settings = t;
}

void CSettings::removeAt(int index)
{
    int size = getSize();
    for (int i = index; i < size ; ++i) {
        m_settings[i] = m_settings[i + 1];
    }
}

void CSettings::removeAll()
{
    if (m_settings) {
        delete [] m_settings;
        m_settings = nullptr;
    }
    m_settings = new SETTING[1];
}
