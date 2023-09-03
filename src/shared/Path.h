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

#ifndef PATH_H
#define PATH_H

#include <string>
#include <inttypes.h>

class IFile;

class CPath
{
public:
    CPath();
    ~CPath();

    void add(uint8_t aim);
    void insertAt(int i, uint8_t aim);
    void removeAt(int i);
    void read(IFile &file);
    void write(IFile & file);
    int getSize() const;
    int getOptions() const;
    void setOptions(int option);
    void forget();
    void debug();
    std::string toHex();
    uint8_t *raw();

    uint8_t & operator[] (int i) const;
    CPath & operator = (const CPath & src);
    bool operator == (const CPath & src);
    bool operator != (const CPath & src);

    static bool isValidAim(uint8_t aim);

    enum {
        PO_NONE = 0,
        PO_DEFAULT = 1
    };

protected:

    enum {
        GROW_BY = 64
    };

    int m_size;
    int m_max;
    int m_options;
    uint8_t *m_path;
};
#endif // PATH_H
