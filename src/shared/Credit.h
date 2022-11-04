/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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
#ifndef __CREDIT_H
#define __CREDIT_H

#include <string>
#include <unordered_map>
#include <list>
class IFile;

#ifndef _LICENSE_H_
#define _LICENSE_H_
#include <string>

namespace lgck {
    namespace License{
        enum LicenseType: uint8_t{
            CC_BY_4_0,
            CC_BY_3_0,
            CC_BY_SA_4_0,
            CC_BY_SA_3_0,
            GPL_3,
            GPL_2,
            OGA_BY_3_0,
            CC0,
            LGPL,
            LGPL_2,
            Count
        };

        struct LicenseDefinition{
            std::string name;
            std::string slug;
            std::string description;
            std::string url;
        };
    };
};

#endif

class Credit
{
public:
    Credit();
    ~Credit();
    bool read(IFile & file);
    bool write(IFile & file);
    std::list<int> & licenses();
    const char* get(const char *el);
    void set(const char *el, const char *v);
    void toString(std::string & s);

protected:
    std::unordered_map<std::string, std::string> m_detail;
    std::list<int> m_licenses;
};

#endif
