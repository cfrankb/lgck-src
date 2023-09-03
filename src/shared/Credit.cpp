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
#include <cstring>
#include "Credit.h"
#include "IFile.h"
#include <cstdint>

const lgck::License::LicenseDefinition licenses[] {
    {
        "Creative Commons Attribution 4.0", "CC-BY 4.0",
        "This license requires you to attribute the author of the content in the way that they specify. Provided the author is properly credited, it is generally safe to use this content in a commercial work.",
        "http://creativecommons.org/licenses/by/4.0/"
    },
    {
        "Creative Commons Attribution 3.0", "CC-BY 3.0",
        "This license requires you to attribute the author of the content in the way that they specify. Provided the author is properly credited, it is generally safe to use this content in a commercial work.",
        "http://creativecommons.org/licenses/by/3.0/"
    },
    {
        "Creative Commons Attribution-Share Alike 4.0", "CC-BY-SA 4.0",
        "This license requires you to release any modifications you make to the art work in question under the same license.",
        "http://creativecommons.org/licenses/by-sa/4.0/"
    },
    {
        "Creative Commons Attribution-Share Alike 3.0", "CC-BY-SA 3.0",
        "This license requires you to release any modifications you make to the art work in question under the same license.",
        "https://creativecommons.org/licenses/by-sa/3.0/"
    },
    {
        "GNU General Public License 3", "GPL 3",
        "This license requires you to release any modifications you make to the art work under the same license.",
        "https://www.gnu.org/licenses/gpl-3.0.html"
    },
    {
        "GNU General Public License 2", "GPL 2",
        "This license, requires you to release any modifications you make to the art work under the same license.",
        "http://www.gnu.org/licenses/old-licenses/gpl-2.0.html"
    },
    {
        "OpenGameArt Attribution 3.0", "OGA-BY 3.0",
        "This license is based on CC-BY 3.0 above, but removes that license's restriction on technical measures that prevent redistribution of a work. A lot of artists feel that the restrictions on technical measures in CC-BY go against the spirit of CC-BY, which is to require attribution without imposing other requirements. TL;DR: Same as CC-BY but without anti-DRM restrictions.",
        "https://opengameart.org/content/oga-by-30-faq"
    },
    {
        "Copyright-Only Dedication (Public Domain)", "CC0",
        "This license is equivalent to the Public Domain. There are therefore no legal concerns with using it, and it is safe to use in any project.",
        "https://creativecommons.org/publicdomain/zero/1.0/"
    },
    {
        "GNU Lesser General Public License 3", "LGPL 3",
        "This license requires you to release any modifications you make to the art work under the same license. Since the FSF has clarified that use of art does not trigger the GPL's linking requirement, the LGPL is effectively the same as the GPL when used for art.",
        "http://www.gnu.org/copyleft/lgpl.html"
    },
    {
        "GNU Lesser General Public License 2", "LGPL 2",
        "This license requires you to release any modifications you make to the art work under the same license. Since the FSF has clarified that use of art does not trigger the GPL's linking requirement, the LGPL is effectively the same as the GPL when used for art.",
        "http://www.gnu.org/licenses/old-licenses/library.txt"
    }
};


Credit::Credit()
{

}

Credit::~Credit()
{

}

bool Credit::read(IFile & file)
{
    m_licenses.clear();
    m_detail.clear();
    uint32_t count = 0;
    file.read(&count, sizeof(uint8_t));
    for (uint32_t i=0; i < count; ++i) {
        uint8_t x;
        file.read(&x, sizeof(uint8_t));
        m_licenses.push_back(x);
    }
    file.read(&count, sizeof(uint8_t));
    for (uint32_t i=0; i < count; ++i) {
        std::string k;
        std::string v;
        file >> k;
        file >> v;
        m_detail[k] = v;
    }
    return true;
}

bool Credit::write(IFile & file)
{
    uint32_t count = m_licenses.size();
    file.write(&count, sizeof(uint8_t));
    for (uint32_t x : m_licenses) {
        file.write(&x, sizeof(uint8_t));
    }
    count = 0;
    for (auto it = m_detail.begin(); it != m_detail.end(); ++it) {
        if (strlen(it->second.c_str())) {
            ++count;
        }
    }
    file.write(&count, sizeof(uint8_t));
    for (auto it = m_detail.begin(); it != m_detail.end(); ++it) {
        file << it->first;
        file << it->second;
    }
    return true;
}

std::list<int> & Credit::licenses()
{
    return m_licenses;
}

const char* Credit::get(const char *el)
{
    return m_detail[el].c_str();
}

void Credit::set(const char *el, const char *v)
{
    m_detail[el] = v;
}

void Credit::toString(std::string & s)
{

}
