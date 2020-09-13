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
#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <unordered_map>
#include <string>

class CScene;
class IFile;
class CGame;
class CLevel;
class CDisplayConfig;
class CCountdown;

class CSnapshot
{
public:

    CSnapshot();
    ~CSnapshot();

    void forget();
    void read(IFile &file);
    void write(IFile & file);
    bool has_snapshot();
    void take(CGame & game);
    bool reload(CGame & game);
    void clear();

protected:
    CScene *m_fw;
    CScene *m_bk;
    CLevel *m_layers;
    CCountdown *m_countdown;
    CDisplayConfig *m_displayConf;
    std::unordered_map<std::string, unsigned long long> m_vars;

    enum {
        VERSION = 0x0001
    };

    friend class CGame;
};

#endif // SNAPSHOT_H
