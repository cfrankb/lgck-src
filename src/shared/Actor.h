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

#ifndef ACTOR_H
#define ACTOR_H

#include "LevelEntry.h"
#include <unordered_map>

class CGame;
class CHitData;
class CInventory;
class CMapEntry;
class CObject;
class CPath;
class CProto;
class IFile;

class CActor : public CLevelEntry
{
public:
    CActor();
    CActor(const CLevelEntry &e);
    CActor(const CProto &p);
    ~CActor();
    CActor & operator = (const CActor &s);
    const CProto & proto();
    const CObject & object();
    void activate();
    void animate();
    void attack(CActor &t);
    void attackPlayer();
    void autoCenter(int aim);
    void autoCenter();
    void callTrigger();
    void callEvent(int event);
    bool canMove(int nAim, bool bActor=false);
    bool canFall();
    void changeTo(int protoId);
    int checkHit();
    int childCount();
    void deactivate();
    void doManage();
    unsigned int getAge();
    void getExtra(int * &extra, int* &index, int &size);
    int getFlags();
    CPath & getPath(int pathDir);
    int getSeed();
    bool getState (int nState);
    int getHP();
    bool hitTest(int aim, CHitData & hitData);
    bool isAntiGravity(const CMapEntry * map);
    bool isActive();
    bool isDead();
    bool isFrozen();
    bool isGoal();
    bool isMonster();
    bool isPlayer();
    bool isPlayerThere (int nAim);
    bool isHidden();
    bool isTagged(int flag);
    bool isVisible();
    bool isSolid(const CMapEntry * map, int aim);
    void kill();
    void land();
    bool map();
    void moveTo(const CActor & src);
    void moveTo(const int x, const int y);
    void move(const int aim);
    void move();
    void read(IFile &file, int version);
    void setHP(int hp);
    int setState(int nState, bool bOn);
    void setTag(int flag, bool bit = true);
    void setOwner(int ownerEntry);
    void spawn();
    void stopAnimation();
    bool tryAnimation(int seq);
    bool tryPath(int pathId, int aim=-1);
    void triggerHitState();
    bool unMap();
    void write(IFile &file);
    CInventory * getInventory();
    const char * getInventoryName();
    void resetInventory();
    void addItem(int protoId, int count=1);
    void removeItem(int protoId);
    bool hasItem(int protoId);
    void set(const char *name, int value);
    int & get(const char *name);
    void set(int i, int value);
    int & get(int i);
    void togglePathPlayback(bool enable);

    enum  {
        TAG_REBORN          = 0x0001,
        TAG_ACTIVATED       = 0x0002
    };

protected:
    std::unordered_map <std::string, int> m_props;
    std::unordered_map <int, int> m_propi;
    unsigned int m_id;       // sprite id
    unsigned int m_seed;     // seed uid
    friend class CGame;
    friend class CScene;
    static void setGame(CGame *game);
    void crash();
    void doManage_PlayerBullet();
    void managePath();
    void followPath();
    void init();
    static CGame * m_game;
};

#endif // ACTOR_H
