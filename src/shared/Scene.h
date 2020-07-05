/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2014  Francois Blanchette

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

#ifndef SCENE_H
#define SCENE_H

#include "Actor.h"
#include "ISurface.h"
#include "../shared/stdafx.h"

class CLayer;
class CGame;
class IFile;

class CScene: public ISurface
{
public:
    CScene();
    ~CScene();

    int add(const CLevelEntry & entry, bool toSpawn=true);
    int add(CActor & actor, bool toSpawn=true);
    void removeAt(int i);
    void forget();
    CActor & operator [] (int i) const;
    virtual CLevelEntry & atIndex (int i) const;
    void setOwner(CGame *game, bool bk);
    virtual int getSize() const;
    int findPlayerEntry();
    void hideSystemObject(bool bHide);
    int countGoals();
    void animate();
    void doManage();
    int whoIs(int x, int y);
    void map();
    void manageAuto();
    int findBySeed(uint32_t seed);
    const CScene & operator = (const CLayer & layer);
    void read(IFile &file);
    void write(IFile & file);
    const CScene & operator = (const CScene & s);
    void notifyClosure();
    void notifyAll(int eventId);

protected:
    CActor **m_actors;
    int m_size;
    int m_max;
    CGame *m_game;
    bool m_bk;

    enum {
        GROWBY = 32,
        VERSION = 0x0002
    };

    unsigned int m_seed;
    static char m_signature[];
};

#endif // SCENE_H
