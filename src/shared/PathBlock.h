/*
    LGCK Builder GUI
    Copyright (C) 1999, 2012  Francois Blanchette

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

#ifndef PATHBLOCK_H
#define PATHBLOCK_H

class IFile;
class CPath;

class CPathBlock
{
public:
    CPathBlock();
    ~CPathBlock();
    void read(IFile &file);
    void write(IFile & file);
    void forget();
    CPath & operator [](int i);
    int addPath();
    void resetPath(int id);
    int copyPath(int id);
    CPath & getPath(int id);
    int duplicate(int id);
    enum {
        PB_PLAYBACK = 1,
        PB_APPLY_ACTIVE = 2,
        PB_ATTACK_PLAYER = 4,
        PB_IGNORE_GRAVITY = 8,
        PB_IGNORE_SOLID = 16,
        PB_LOOP = 32,
        PB_RETURN_ORIGIN = 64,
        PB_CUSTOM_HANDLER = 128
    };

protected:
    int findFree();
    enum {
        GROW_BY = 64,
        VERSION = 0
    };
    int m_size;
    int m_max;
    CPath *m_paths;
};

#endif // PATHBLOCK_H
