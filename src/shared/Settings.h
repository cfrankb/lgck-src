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

#ifndef CSETTINGS_H
#define CSETTINGS_H
#include <string>
class IFile;

class CSettings
{
public:
    CSettings();
    ~CSettings();

    typedef struct {
        std::string param;
        std::string value;
        int valueInt;
    } SETTING;

    SETTING & operator [] (int i) { return m_settings [i]; }

    bool parseSettings(const char * strText);
    void outputSettings(std::string & strText);
    void add(const char * param, const char * value);
    SETTING & operator [] (const char * param);

    void copySettings(const SETTING * defaults);
    CSettings & operator = (CSettings & s);
    CSettings & operator >> (IFile & file);
    CSettings & operator << (IFile & file);
    void removeAt(int index);
    void removeAll();
    int getSize();
    void setReplaceAll( bool b) {
        m_bReplaceAll = b;
    }

protected:
    SETTING *m_settings;
    bool m_bReplaceAll;
    int calculateCost(const std::string & s);
    void addSlashes2(const std::string & s, char *t);
    void removeSlashes(const std::string & s, std::string & t);
};

#endif // CSETTINGS_H
