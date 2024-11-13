/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2013  Francois Blanchette

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
#include <string>
#include <cstdlib>
#include <cassert>
#include "Actor.h"
#include "Animation.h"
#include "Attacker.h"
#include "Drone.h"
#include "Frame.h"
#include "Game.h"
#include "Inventory.h"
#include "Map.h"
#include "Monster.h"
#include "Path.h"
#include "PathBlock.h"
#include "Proto.h"
#include "Scene.h"
#include "Tasks.h"
#include "vlamits3.h"
#include "Level.h"
#include "GameEvents.h"
#include "Extra.h"
#include "IFile.h"
#include "StringTable.h"

CGame *CActor::m_game = nullptr;

void CActor::read(IFile &file, int)
{
    int size = sizeof(CLevelEntry);
    int oldSize;
    file.read(&oldSize, sizeof(int));
    assert(size == oldSize);
    file.read(this, size);
    file.read(&m_id, sizeof(int));
    file.read(&m_seed, sizeof(int));
    file.read(&size, sizeof(int));
    m_props.clear();
    for (int i = 0; i < size; ++i)
    {
        std::string key;
        file >> key;
        int val = 0;
        file.read(&val, sizeof(val));
        m_props[key] = val;
    }
    // TODO: revisit this
    m_propi.clear();
    file.read(&size, sizeof(int));
    for (int i = 0; i < size; ++i)
    {
        int key = 0;
        int val = 0;
        file.read(&key, sizeof(short));
        file.read(&val, sizeof(int));
        m_propi[key] = val;
    }
}

void CActor::write(IFile &file)
{
    int size = sizeof(CLevelEntry);
    file.write(&size, sizeof(int));
    file.write(this, size);
    file.write(&m_id, sizeof(int));
    file.write(&m_seed, sizeof(int));
    size = m_props.size();
    file.write(&size, sizeof(int));
    for (const auto & kv : m_props)
    {
        std::string key = kv.first;
        int val = kv.second;
        file << key;
        file.write(&val, sizeof(int));
    }
    // TODO: revisit this
    size = 0;
    for (auto kv : m_propi)
    {
        if (kv.second)
        {
            ++size;
        }
    }
    file.write(&size, sizeof(int));
    for (auto kv : m_propi)
    {
        int key = kv.first;
        int val = kv.second;
        if (val)
        {
            file.write(&key, sizeof(short));
            file.write(&val, sizeof(int));
        }
    }
}

CActor &CActor::operator=(const CActor &s)
{
    int size = sizeof(CLevelEntry);
    memcpy(static_cast<void *>(this), &s, size);
    m_id = s.m_id;
    m_seed = s.m_seed;
    m_props.clear();
    for (const auto & kv : s.m_props)
    {
        std::string key = kv.first;
        m_props[key] = kv.second;
    }
    m_propi = s.m_propi;
    return *this;
}

void CActor::copyFrom(const CActor &s)
{
    *(this) = s;
}

void CActor::init()
{
    m_id = 0;
    m_propi[lgck::EXTRA_TAGFLAGS] = 0;
    m_propi[lgck::EXTRA_INVENTORY] = 0;
}

CActor::CActor() : CLevelEntry()
{
    init();
}

CActor::CActor(const CLevelEntry &e) : CLevelEntry(e)
{
    init();
}

CActor::CActor(const CProto &p) : CLevelEntry(p)
{
    init();
}

CActor::~CActor()
{
}

void CActor::spawn()
{
    if (m_nProto >= 0)
    {
        const CProto &p = proto();
        m_propi[lgck::EXTRA_HP] = p.m_nHP;
        m_propi[lgck::EXTRA_ANIMSPEED] = p.m_nAniSpeed;
        if (!p.isPlayer() && !isTagged(TAG_REBORN))
        {
            m_propi[lgck::EXTRA_LIVES] = p.m_nRebirths;
        }
        setTag(TAG_ACTIVATED, p.getOption(CProto::OPTION_INACTIVE) == 0);
        if (!CGame::isBackgroundClass(p.m_nClass) &&
            p.getOption(CProto::OPTION_INACTIVE) == 0)
        {
            callEvent(CObject::EO_ACTIVATE);
        }
    }
    else
    {
        m_propi[lgck::EXTRA_ANIMSPEED] = CGame::INVALID;
    }
    m_nStateFlag = 0;

    m_propi[lgck::EXTRA_TIMEOUT] = 0;
    m_propi[lgck::EXTRA_ORGX] = m_nX;
    m_propi[lgck::EXTRA_ORGY] = m_nY;
    m_propi[lgck::EXTRA_ORGPROTO] = m_nProto;

    // reset animation seq
    m_propi[lgck::EXTRA_ANIMPTR] = 0;
    m_propi[lgck::EXTRA_ANIMSEQ] = CGame::INVALID;
    m_propi[lgck::EXTRA_DEATHINDICATOR] = CGame::DI_NONE;

    // path
    m_propi[lgck::EXTRA_FLAGS] = 0;
    if (m_path && (m_playback & CPathBlock::PB_PLAYBACK) && tryPath(CObject::PS_SPRITE_CUSTOM))
    {
        m_propi[lgck::EXTRA_PATHDIR] = CObject::PS_SPRITE_CUSTOM;
    }
    else
    {
        m_propi[lgck::EXTRA_PATHDIR] = CGame::INVALID;
        m_propi[lgck::EXTRA_PATHPTR] = 0;
    }
    m_propi[lgck::EXTRA_BULLETCOUNT] = 0;
    m_propi[lgck::EXTRA_AGE] = 0;
    m_propi[lgck::EXTRA_ACTIVE] = 0;
    m_propi[lgck::EXTRA_OWNER] = 0;
    if (m_nProto > 0 && !proto().isBkClass())
    {
        tryAnimation(CObject::AS_DEFAULT);
        callEvent(CObject::EO_SPAWN);
    }
}

const CProto &CActor::proto()
{
    return m_game->m_arrProto[m_nProto];
}

CProto &CActor::getProto()
{
    return m_game->m_arrProto[m_nProto];
}

const CObject &CActor::object()
{
    return m_game->m_arrProto.getObject(m_nProto);
}

int CActor::setState(int nState, bool bOn)
{
    if (bOn)
    {
        m_nStateFlag |= nState;
    }
    else
    {
        m_nStateFlag -= m_nStateFlag & nState;
    }
    return m_nStateFlag;
}

bool CActor::getState(int nState)
{
    return (m_nStateFlag & nState) != 0;
}

bool CActor::tryAnimation(int seq)
{
    if (seq >= 0 && seq <= CObject::AS_MAX_VALUE)
    {
        const CAnimation &animation = object().getAnimation(seq);
        if (animation.getSize())
        {
            m_propi[lgck::EXTRA_ANIMSEQ] = seq;
            m_propi[lgck::EXTRA_ANIMPTR] = 0;
            return true;
        }
    }
    else
    {
        CLuaVM::debugv("-- tryAnimation() seq `0x%x` out of range for `%d`.", seq, m_id);
    }
    return false;
}

void CActor::stopAnimation()
{
    m_propi[lgck::EXTRA_ANIMSEQ] = CGame::INVALID;
    m_propi[lgck::EXTRA_ANIMPTR] = 0;
}

void CActor::kill()
{
    if (m_nProto < 0)
    {
        m_nProto = 0;
        return;
    }

    const CProto &p = proto();
    if (p.isPlayer())
    {
        setState(CHitData::STATE_DEAD, true);
        set(lgck::EXTRA_HP, 0);
        return;
    }

    // do not remove
    const bool isPickUp = isPickUpTrigger();
    const bool isGoalObj = isGoal();

    const CProto &newproto = m_game->m_arrProto[p.m_nChProto];
    m_propi[lgck::EXTRA_OWNER] = 0;
    // test if a rebirth is possible
    if (m_nProto > 0 && !p.isPlayer() && m_propi[lgck::EXTRA_LIVES])
    {
        --m_propi[lgck::EXTRA_LIVES];
        CTask *task = new CTask;
        CActor &actor = *this;
        actor.setTag(TAG_REBORN);
        switch (p.m_rebirthLocation)
        {
        case CProto::REBIRTH_ORIGIN:
            actor.m_nX = m_propi[lgck::EXTRA_ORGX];
            actor.m_nY = m_propi[lgck::EXTRA_ORGY];
            break;
        case CProto::REBIRTH_RANDOM:
            actor.m_nX = (::rand() % m_game->BUFFERLEN) & 0xfff8;
            actor.m_nY = (::rand() % m_game->BUFFERHEI) & 0xfff8;
            break;
        }
        actor.m_nProto = actor.m_propi[lgck::EXTRA_ORGPROTO];
        task->m_actor = actor;
        task->m_task = CTask::TASK_REBIRTH;
        task->m_ticks = m_game->var("ticks") + p.m_nRbDelay * 20;
        m_game->m_tasks->add(task);
    }

    if (p.getOption(CProto::OPTION_NO_SHIFT_AT_DEATH))
    {
        m_nProto = CGame::PROTO_DEAD_WOOD;
    }
    else
    {
        m_nProto = p.m_nChProto;
        m_nFrameNo = newproto.m_nFrameNo;
        m_nFrameSet = newproto.m_nFrameSet;
        m_nAim = p.m_nDefaultAim;
        m_propi[lgck::EXTRA_ANIMSEQ] = CObject::AS_DEFAULT;
        m_propi[lgck::EXTRA_ANIMPTR] = 0;
    }

    if (isGoal())
    {
        m_nTriggerKey ^= TRIGGER_GOAL;
    }

    if (isFrozen())
    {
        m_nTriggerKey ^= TRIGGER_FROZEN;
    }

    if (p.m_nPoints != 0)
    {
        // this sprite will give you points
        CActor newSprite;
        newSprite.m_nFrameNo = p.m_nPoints;
        newSprite.m_nFrameSet = 0;
        newSprite.m_nProto = CGame::PROTO_POINTS;
        newSprite.moveTo(*this);
        newSprite.m_nAim = CGame::UP;
        newSprite.m_propi[lgck::EXTRA_LIVES] = 0;
        m_game->addToScore(m_game->m_arrPoints[p.m_nPoints]);
        m_game->m_tasks->createTask(newSprite);
    }

    if (p.m_nBonusHP)
    {
        m_game->addToHP(p.m_nBonusHP);
    }
    m_game->m_counters["coins"] += p.m_coinsBonus;
    m_game->m_counters["lives"] += p.m_livesBonus;
    m_game->m_counters["ammo"] += p.m_ammoBonus;

    int nChSound = p.m_nChSound;
    if (nChSound)
    {
        m_game->playSound(nChSound);
    }

    if (isPickUp)
    {
        m_game->callGameEvent(CGameEvents::EG_PICKUP);
    }

    if (isGoalObj)
    {
        m_game->callGameEvent(CGameEvents::EG_GOAL_KILLED);
        m_game->callLvEvent(CLevel::EL_GOAL_KILLED);
    }
}

void CActor::moveTo(const CActor &src)
{
    m_nX = src.m_nX;
    m_nY = src.m_nY;
}

void CActor::moveTo(const int x, const int y)
{
    m_nX = x;
    m_nY = y;
}

void CActor::callTrigger()
{
    // if OPTION_NO_TRIGGER_CALL flag is set this
    // sprite cannot call a trigger.
    if (!proto().getOption(CProto::OPTION_NO_TRIGGER_CALL))
    {
        m_game->callTrigger(m_nTriggerKey & TRIGGER_KEYS);
    }
}

void CActor::setGame(CGame *game)
{
    m_game = game;
}

bool CActor::isMonster()
{
    return proto().isMonster();
}

bool CActor::isVisible()
{
    CFrameSet &set = m_game->toFrameSet(m_nFrameSet);
    CFrame *pFrame = set[m_nFrameNo];
    int x = m_nX - m_game->_mx();
    int y = m_nY - m_game->_my();
    return !((m_nTriggerKey & TRIGGER_HIDDEN) ||
             (x + pFrame->len() <= 0) ||
             (x >= m_game->m_screenLen) ||
             (y + pFrame->len() <= 0) ||
             (y >= m_game->m_screenHei));
}

void CActor::attackPlayer()
{
    CActor &player = m_game->getPlayer();
    const CProto protoActor = proto();
    const CProto protoPlayer = player.proto();
    if (!player.getState(CHitData::STATE_HIT) && !player.getState(CHitData::STATE_DEAD) && ((!protoActor.m_nPowerLevel) || (protoActor.m_nPowerLevel >= protoPlayer.m_nPowerLevel)))
    {
        if (protoActor.m_nPowerLevel <= protoPlayer.m_nPowerLevel)
        {
            m_game->triggerPlayerHitState();
            player.m_propi[lgck::EXTRA_TIMEOUT] = m_game->var("CONST_TIMEOUT");
            m_game->addToHP(-protoActor.m_nDamages);
        }
        else
        {
            m_game->killPlayer(player);
        }
    }
}

void CActor::callEvent(int event)
{
    m_game->callObjEvent(m_id, event);
}

bool CActor::canMove(int nAim, bool bActor)
{
    CFrame &frame = m_game->toFrame(*this);
    const Size sx = CMap::size(&frame);
    Pos p = m_game->map().toMap(m_nX, m_nY);
    const CProto &po = m_game->m_arrProto[m_nProto];
    switch (nAim)
    {
    case CGame::UP:
        if (m_nY == 0)
        {
            if (m_game->getWrap() & CLevel::WRAP_UP)
            {
                CActor &t = *this;
                t.moveTo(m_nX, m_game->BUFFERHEI);
                if (t.canMove(CGame::UP, bActor))
                {
                    return true;
                }
            }
            if (!m_game->gravity() || !po.isPlayer())
            {
                return false;
            }
        }
        --p.y;
        break;

    case CGame::DOWN:
    case CGame::FALL:
        if (m_nY >= m_game->BUFFERHEI - frame.hei())
        {
            if (m_game->getWrap() & CLevel::WRAP_DOWN)
            {
                CActor t;
                t.copyFrom(*this);
                t.move(CGame::UP);
                if (t.canMove(CGame::DOWN, bActor))
                {
                    return true;
                }
            }

            return false;
        }
        p.y += sx.hei;
        break;

    case CGame::LEFT:
        if (m_nX == 0)
        {
            CActor t;
            t.copyFrom(*this);
            t.moveTo(m_game->BUFFERLEN, m_nY);
            if ((m_game->getWrap() & CLevel::WRAP_LEFT) && t.canMove(CGame::LEFT, bActor))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        --p.x;
        break;

    case CGame::RIGHT:
        if (m_nX >= m_game->BUFFERLEN - frame.len())
        {
            CActor t;
            t.copyFrom(*this);
            t.move(CGame::LEFT);
            if ((m_game->getWrap() & CLevel::WRAP_RIGHT) && t.canMove(CGame::RIGHT, bActor))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        p.x += sx.len;
        break;

    default:
        return false;
    }

    CMap &m = m_game->map();
    if (nAim == CGame::UP || nAim == CGame::DOWN || nAim == CGame::FALL)
    {
        if (p.y >= 0)
        {
            for (int x = 0; x < sx.len; ++x)
            {
                CMapEntry &map = m.at(p.x + x, p.y);
                if (isSolid(&map, nAim))
                {
                    return false;
                }
                if ((map.m_nBkClass == CLASS_STOP) && bActor)
                {
                    return false;
                }
            }
        }
    }
    else
    {
        for (int y = 0; y < sx.hei; ++y)
        {
            if ((p.y + y) >= 0)
            {
                CMapEntry &map = m.at(p.x, p.y + y);
                if (isSolid(&map, nAim))
                {
                    return false;
                }
                if ((map.m_nBkClass == CLASS_STOP) && bActor)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool CActor::canFall()
{
    if (!m_game->gravity())
    {
        return false;
    }
    const CProto &pr = proto();
    if (pr.getOption(CProto::OPTION_NO_GRAVITY))
    {
        return false;
    }
    CFrame &frame = m_game->toFrame(*this);
    const Size sx = CMap::size(frame);
    CMap &m = m_game->map();
    const Pos p = m.toMap(m_nX, m_nY);
    if (!canMove(CGame::FALL, false))
    {
        return false;
    }
    if (m_nY < 0)
        return true;
    int i = 0;
    int ii = 0;
    for (int y = 0; y < sx.hei; ++y)
    {
        for (int x = 0; x < sx.len; ++x)
        {
            CMapEntry &map = m.at(p.x + x, p.y + y);
            if ((map.m_nBkClass && map.m_nBkClass < 0xa) || isAntiGravity(&map))
            {
                ++i;
            }
        }
    }
    for (int x = 0; x < sx.len; ++x)
    {
        CMapEntry &map = m.at(p.x + x, p.y + sx.hei);
        if ((map.m_nBkClass && map.m_nBkClass < 0xa) || isAntiGravity(&map))
        {
            ++ii;
        }
    }
    int entrySize = sx.len * sx.hei;
    return (i < entrySize) && (ii < sx.len);
}

bool CActor::map()
{
    if (m_nY < 0 || m_nProto & CGame::PROTO_SPECIAL_MASK)
        return false;
    CMap &m = m_game->map();
    const CProto &po = proto();
    if (po.m_nClass != 0 && (m_nTriggerKey & TRIGGER_HIDDEN) == 0)
    {
        CFrame &frame = m_game->toFrame(*this);
        const Size sx = CMap::size(frame);
        const Pos p = m.toMap(m_nX, m_nY);
        for (int y = 0; y < sx.hei; ++y)
        {
            if (m_nY + y >= 0)
            {
                for (int x = 0; x < sx.len; ++x)
                {
                    CMapEntry &map = m.at(p.x + x, p.y + y);
                    if (po.m_bNoSmartMap || frame.map(x, y))
                    {
                        if (po.isPlayer())
                        {
                            map.m_bPlayer = true;
                        }
                        else
                        {
                            if (po.m_nClass < CLASS_GENERIC_COS)
                            {
                                map.mapFw(po.m_nClass, m_id);
                            }
                            else
                            {
                                map.mapAc(po.m_nClass, m_id);
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool CActor::unMap()
{
    if (m_nProto & CGame::PROTO_SPECIAL_MASK)
    {
        return false;
    }

    CMap &m = m_game->map();
    const int nClass = proto().m_nClass;
    if (nClass != 0)
    {
        CFrame &frame = m_game->toFrame(*this);
        const Size sx = CMap::size(frame);
        const Pos p = m.toMap(m_nX, m_nY);
        for (int y = 0; y < sx.hei; ++y)
        {
            for (int x = 0; x < sx.len; ++x)
            {
                CMapEntry &map = m.at(p.x + x, p.y + y);
                if (map.m_bPlayer && nClass == CLASS_PLAYER_OBJECT)
                {
                    map.m_bPlayer = false;
                }
                map.unMapFw(m_id);
                if (map.m_nAcEntry == m_id)
                {
                    map.unMapAc(m_id);
                }
            }
        }
    }
    return true;
}

void CActor::triggerHitState()
{
    callEvent(CObject::EO_HURT);
    setState(CHitData::STATE_HIT, true);
    tryAnimation(CObject::AS_HURT + m_nAim);
    m_propi[lgck::EXTRA_TIMEOUT] = m_game->var("CONST_TIMEOUT");
}

void CActor::move()
{
    move(m_nAim);
}

void CActor::move(const int aim)
{
    const CMap &map = m_game->map();
    Pos p;
    switch (aim)
    {
    case CGame::UP:
        if ((m_game->getWrap() & CLevel::WRAP_UP) && !m_nY)
        {
            p = map.wScreen(m_nX, -CMap::GRID);
            m_nY = p.y;
        }
        else
        {
            moveBy(0, -CMap::GRID);
        }
        break;

    case CGame::DOWN:
        if (m_nY < 0)
        {
            moveBy(0, CMap::GRID);
        }
        else
        {
            p = map.wScreen(m_nX, m_nY + CMap::GRID);
            moveTo(p.x, p.y);
        }
        break;

    case CGame::LEAP_LEFT:
        moveBy(0, -CMap::GRID);
        /* fall through */
    case CGame::LEFT:
        if (m_nY < 0)
        {
            moveBy(-CMap::GRID, 0);
        }
        else
        {
            p = map.wScreen(m_nX - CMap::GRID, m_nY);
            moveTo(p.x, p.y);
        }
        break;

    case CGame::LEAP_RIGHT:
        moveBy(0, -CMap::GRID);
        /* fall through */
    case CGame::RIGHT:
        if (m_nY < 0)
        {
            moveBy(CMap::GRID, 0);
        }
        else
        {
            p = map.wScreen(m_nX + CMap::GRID, m_nY);
            moveTo(p.x, p.y);
        }
    }
}

void CActor::land()
{
    callEvent(CObject::EO_LAND);
    const CProto &p = proto();
    if (p.m_nMaxFall && m_propi[lgck::EXTRA_FALLHEIGHT] > p.m_nMaxFall)
    {
        callEvent(CObject::EO_DEATH);
        if (!tryAnimation(CObject::AS_DEAD + m_nAim))
        {
            stopAnimation();
        }
        setState(CHitData::STATE_DEAD, true);
        m_propi[lgck::EXTRA_HP] = 0;
    }

    m_propi[lgck::EXTRA_FALLHEIGHT] = 0;
}

bool CActor::isDead()
{
    return getState(CHitData::STATE_DEAD);
}

bool CActor::isHidden()
{
    return (m_nTriggerKey & TRIGGER_HIDDEN) != 0;
}

void CActor::changeTo(int protoId)
{
    const CProto &proto = m_game->m_arrProto[protoId];
    stopAnimation();
    m_nProto = protoId;
    m_nFrameSet = proto.m_nFrameSet;
    m_nFrameNo = proto.m_nFrameNo;
    tryAnimation(CObject::AS_DEFAULT);
}

void CActor::attack(CActor &t)
{
    const CProto &p = proto();
    const CProto &tp = t.proto();
    if (p.m_nPowerLevel >= tp.m_nPowerLevel)
    {
        t.m_propi[lgck::EXTRA_HP] -= p.m_nDamages;
        if (t.m_propi[lgck::EXTRA_HP] <= 0)
        {
            t.setState(CHitData::STATE_DEAD, true);
            if (!t.tryAnimation(CObject::AS_DEAD + t.m_nAim))
            {
                t.stopAnimation();
            }
            t.callEvent(CObject::EO_DEATH);
            t.callTrigger();
        }
        else
        {
            t.callEvent(CObject::EO_HURT);
        }
    }
}

bool CActor::isPlayerThere(int nAim)
{
    CFrame &frame = m_game->toFrame(*this);
    Size sx = CMap::size(frame);
    CMap &rmap = m_game->map();
    Pos p = rmap.toMap(m_nX, m_nY);
    switch (nAim)
    {
    case CGame::UP:
        --p.y;
        if (m_nY == 0)
            return false;
        break;

    case CGame::DOWN:
        p.y += sx.hei;
        if (m_nY >= m_game->BUFFERHEI - frame.hei())
            return false;
        break;

    case CGame::LEFT:
        --p.x;
        if (m_nX == 0)
            return false;
        break;

    case CGame::RIGHT:
        p.x += sx.len;
        if (m_nX >= m_game->BUFFERLEN - frame.len())
            return false;
        break;

    default:
        return false;
    }

    if (nAim == CGame::UP || nAim == CGame::DOWN)
    {
        for (int x = 0; x < sx.len; ++x)
        {
            CMapEntry &map = rmap.at(p.x + x, p.y);
            if (map.m_bPlayer)
            {
                return true;
            }
        }
    }
    else
    {
        for (int y = 0; y < sx.hei; ++y)
        {
            CMapEntry &map = rmap.at(p.x, p.y + y);
            if (map.m_bPlayer)
            {
                return true;
            }
        }
    }
    return false;
}

bool CActor::hitTest(int aim, CHitData &hitData)
{
    CFrame &frame = m_game->toFrame(*this);
    const Size sx = CMap::size(frame);
    CMap &rmap = m_game->map();
    Pos p = rmap.toMap(m_nX, m_nY);

    switch (aim)
    {
    case CGame::UP:
        --p.y;
        if (m_nY == 0)
            return false;
        break;

    case CGame::DOWN:
        p.y += sx.hei;
        if (m_nY >= m_game->BUFFERHEI - frame.hei())
            return false;
        break;

    case CGame::LEFT:
        --p.x;
        if (m_nX == 0)
            return false;
        break;

    case CGame::RIGHT:
        p.x += sx.len;
        if (m_nX >= m_game->BUFFERLEN - frame.len())
            return false;
        break;
    }

    if (aim == CGame::UP || aim == CGame::DOWN)
    {
        for (int x = 0; x < sx.len; ++x)
        {
            CMapEntry &map = rmap.at(p.x + x, p.y);
            hitData.add(map);
        }
    }
    else
    {
        if (aim == CGame::LEFT || aim == CGame::RIGHT)
        {
            for (int y = 0; y < sx.hei; ++y)
            {
                CMapEntry &map = rmap.at(p.x, p.y + y);
                hitData.add(map);
            }
        }
        else
        {
            for (int y = 0; y < sx.hei; ++y)
            {
                for (int x = 0; x < sx.len; ++x)
                {
                    CMapEntry &map = rmap.at(p.x + x, p.y + y);
                    hitData.add(map);
                }
            }
        }
    }
    return true;
}

CPath &CActor::getPath(int pathDir)
{
    if (pathDir != CObject::PS_SPRITE_CUSTOM)
    {
        return object().getPath(pathDir);
    }
    else
    {
        return m_game->getPaths()->getPath(m_path);
    }
}

bool CActor::tryPath(int pathId, int aim)
{
    const CPath &path = getPath(pathId);
    if (path.getSize())
    {
        int animSeqBase = CObject::AS_MOVE;
        if (aim != CObject::PS_SPRITE_CUSTOM)
        {
            if ((pathId & CObject::PS_FILTER) == CObject::PS_JUMP)
            {
                setState(CHitData::STATE_JUMP, true);
                callEvent(CObject::EO_JUMP);
                animSeqBase = CObject::AS_JUMP;
            }
            else
            {
                callEvent(CObject::EO_MOVE);
            }
        }
        else
        {
            aim = path[0];
            callEvent(CObject::EO_MOVE);
        }
        if (aim != CGame::INVALID && m_propi[lgck::EXTRA_PATHDIR] == CGame::INVALID)
        {
            tryAnimation(animSeqBase + aim);
            m_nAim = aim;
        }
        m_propi[lgck::EXTRA_PATHDIR] = pathId;
        m_propi[lgck::EXTRA_PATHPTR] = 0;
        return true;
    }
    return false;
}

void CActor::followPath()
{
    if (m_propi[lgck::EXTRA_PATHDIR] != CGame::INVALID)
    {
        CPath &path = getPath(m_propi[lgck::EXTRA_PATHDIR]);
        if (m_propi[lgck::EXTRA_PATHPTR] >= path.getSize())
        {
            m_propi[lgck::EXTRA_PATHDIR] = CGame::INVALID;
            return;
        }

        int aim = path[m_propi[lgck::EXTRA_PATHPTR]];
        if (aim != CGame::PAUSE &&
            (m_playback & CPathBlock::PB_IGNORE_SOLID || canMove(aim, isMonster())))
        {
            m_nAim = aim;
            unMap();
            tryAnimation(CObject::AS_MOVE + aim);
            move(aim);
            map();
        }

        /*
                PB_PLAYBACK = 1,
                PB_APPLY_ACTIVE = 2,
                PB_ATTACK_PLAYER = 4,
                PB_IGNORE_GRAVITY = 8,
                PB_IGNORE_SOLID = 16,
                PB_LOOP = 32,
                PB_RETURN_ORIGIN = 64,
                PB_CUSTOM_HANDLER = 128*/

        ++m_propi[lgck::EXTRA_PATHPTR];
        if (m_propi[lgck::EXTRA_PATHPTR] >= path.getSize())
        {
            if (m_playback & CPathBlock::PB_RETURN_ORIGIN)
            {
                unMap();
                m_nX = m_propi[lgck::EXTRA_ORGX];
                m_nY = m_propi[lgck::EXTRA_ORGY];
                map();
            }
            if (m_playback & CPathBlock::PB_LOOP)
            {
                m_propi[lgck::EXTRA_PATHPTR] = 0;
            }
            else
            {
                m_propi[lgck::EXTRA_PATHDIR] = CGame::INVALID;
            }
        }
    }
}

void CActor::managePath()
{
    if (m_propi[lgck::EXTRA_PATHDIR] == CGame::INVALID)
    {
        setState(CHitData::STATE_JUMP, false);
        return;
    }

    CPath &path = getPath(m_propi[lgck::EXTRA_PATHDIR]);
    if (m_propi[lgck::EXTRA_PATHPTR] >= path.getSize())
    {
        setState(CHitData::STATE_JUMP, false);
        m_propi[lgck::EXTRA_PATHDIR] = CGame::INVALID;
        return;
    }

    while (path[m_propi[lgck::EXTRA_PATHPTR]] == CGame::SEPARATOR)
    {
        ++m_propi[lgck::EXTRA_PATHPTR];
        if (m_propi[lgck::EXTRA_PATHPTR] >= path.getSize())
        {
            setState(CHitData::STATE_JUMP, false);
            m_propi[lgck::EXTRA_PATHDIR] = CGame::INVALID;
            return;
        }
    }

    int nAim = path[m_propi[lgck::EXTRA_PATHPTR]];
    bool bcanWalk = canMove(nAim);

    CFrame &frame = m_game->toFrame(m_nFrameSet, m_nFrameNo);

    switch (nAim)
    {
    case CGame::UP:
        bcanWalk = m_nY >= 0 ? bcanWalk : true;
        break;

    case CGame::DOWN:
        bcanWalk = (m_nY + frame.hei() < m_game->BUFFERHEI) && bcanWalk;
        break;

    case CGame::LEFT:
        bcanWalk = (m_nX > 0) && bcanWalk;
        break;

    case CGame::RIGHT:
        bcanWalk = (m_nX + frame.len() < m_game->BUFFERLEN) && bcanWalk;
        break;

    case CGame::PAUSE:
        bcanWalk = true;
        break;

    default:
        bcanWalk = false;
        break;
    }

    if (bcanWalk)
    {
        if (nAim != CGame::PAUSE)
        {
            unMap();
            move(nAim);
            map();
        }

        ++m_propi[lgck::EXTRA_PATHPTR];
        if (m_propi[lgck::EXTRA_PATHPTR] >= path.getSize())
        {
            setState(CHitData::STATE_JUMP, false);
            m_propi[lgck::EXTRA_PATHDIR] = CGame::INVALID;
            return;
        }
    }
    else
    {
        setState(CHitData::STATE_JUMP, false);
        m_propi[lgck::EXTRA_PATHDIR] = CGame::INVALID;
        crash();
    }
}

void CActor::doManage()
{
    ++m_propi[lgck::EXTRA_AGE];
    CActor &player = m_game->getPlayer();
    const CProto &po = proto();
    int nAim = m_nAim;
    int ticks = m_game->getTicks();

    if ((m_nProto & CGame::PROTO_POINTS) == CGame::PROTO_POINTS)
    {
        if (ticks % 8 == 0)
        {
            if (m_nY != 0 && isVisible())
            {
                m_nY -= 8;
            }
            else
            {
                m_nProto = 0;
                m_nFrameNo = 0;
                m_nFrameSet = 0;
            }
        }
        return;
    }

    if (!(m_nTriggerKey & (TRIGGER_HIDDEN | TRIGGER_FROZEN)) && !getState(CHitData::STATE_DEAD) && m_nProto != 0)
    {
        if (po.m_nClass != CLASS_PLAYER_OBJECT &&
            m_nProto > 0)
        {
            callEvent(CObject::EO_HANDLER);
        }
        if (!isActive())
        {
            if (isVisible() && !po.getOption(CProto::OPTION_INACTIVE_USERDEF))
            {
                activate();
            }
        }

        if (m_path && m_propi[lgck::EXTRA_PATHDIR] == CObject::PS_SPRITE_CUSTOM)
        {
            if ((po.m_nMoveSpeed == 0) || (ticks % (po.m_nMoveSpeed) == 0))
            {
                followPath();
            }
            // TODO: revisit this
            if (m_playback & CPathBlock::PB_ATTACK_PLAYER &&
                isPlayerThere(m_nAim) && isMonster())
            {
                attackPlayer();
            }
            return;
        }

        switch (po.m_nClass)
        {

        case CLASS_GENERIC_COS:
            static_cast<CMonster *>(this)->doManage();
            break;

        case CLASS_DRONE_LEFT_RIGHT:
        case CLASS_DRONE_UP_DOWN:
            static_cast<CDrone *>(this)->doManage();
            break;

        case CLASS_WACKER_LEFT_RIGHT:
        case CLASS_PLATFORM_LEFT_RIGHT:
            unMap();
            if (nAim == CGame::UP || nAim == CGame::DOWN)
            {
                nAim += CGame::LEFT;
            }
            if ((po.m_nMoveSpeed == 0) || (ticks % (po.m_nMoveSpeed) == 0))
            {
                if (canMove(nAim, true))
                {
                    if (isPlayerThere(CGame::UP) && po.m_nClass != CLASS_WACKER_LEFT_RIGHT)
                    {
                        if (player.canMove(nAim, false))
                        {
                            player.unMap();
                            player.move(nAim);
                            player.map();
                            player.autoCenter(nAim);
                            player.autoCenter(nAim);
                        }
                        else
                        {
                            m_game->killPlayer(player);
                        }
                    }
                    else
                    {
                        if (isPlayerThere(nAim))
                        {
                            if (player.canMove(nAim, false))
                            {
                                player.unMap();
                                player.move(nAim);
                                player.map();
                                player.autoCenter(nAim);
                                player.autoCenter(nAim);
                            }
                            else
                            {
                                m_game->killPlayer(player);
                            }
                        }
                    }
                    move(nAim);
                    m_nAim = nAim;
                }
                else
                {
                    m_nAim = nAim ^ 1;
                }
            }
            map();
            if (m_propi[lgck::EXTRA_ANIMSEQ] == CGame::INVALID || CObject::AS_MOVE + m_nAim != m_propi[lgck::EXTRA_ANIMSEQ])
            {
                tryAnimation(CObject::AS_MOVE + m_nAim);
            }
            break;

        case CLASS_WACKER_UP_DOWN:
        case CLASS_PLATFORM_UP_DOWN:
            unMap();
            if (!po.m_nMoveSpeed || (ticks % (po.m_nMoveSpeed) == 0))
            {
                if (canMove(nAim, true))
                {
                    if (isPlayerThere(nAim))
                    {
                        if (player.canMove(nAim, false))
                        {
                            player.unMap();
                            player.move(nAim);
                            player.map();
                            player.autoCenter(nAim);
                            player.autoCenter(nAim);
                        }
                        else
                        {
                            m_game->killPlayer(player);
                        }
                    }

                    if ((nAim == CGame::DOWN) && (isPlayerThere(CGame::UP)))
                    {
                        player.unMap();
                        player.move(nAim);
                        player.map();
                        player.autoCenter(nAim);
                        player.autoCenter(nAim);
                    }
                    move(nAim);
                    m_nAim = nAim;
                }
                else
                {
                    m_nAim = nAim ^ 1;
                }
            }
            map();
            if (m_propi[lgck::EXTRA_ANIMSEQ] == CGame::INVALID || CObject::AS_MOVE + m_nAim != m_propi[lgck::EXTRA_ANIMSEQ])
            {
                tryAnimation(CObject::AS_MOVE + m_nAim);
            }

            break;

        case CLASS_PLAYER_BULLET:
            doManage_PlayerBullet();
            break;

        case CLASS_CREATURE_BULLET:
            if (!po.m_nMoveSpeed || (ticks % (po.m_nMoveSpeed) == 0))
            {
                if (isPlayerThere(m_nAim))
                {
                    attackPlayer();
                    unMap();
                    move();
                    callEvent(CObject::EO_DEATH);
                    kill();
                    break;
                }

                if (canMove(m_nAim))
                {
                    unMap();
                    move();
                    map();
                }
                else
                {
                    unMap();
                    callEvent(CObject::EO_DEATH);
                    kill();
                }
            }
            break;

        case CLASS_VAMPIRE_PLANT_SOLID:
            for (nAim = 0; nAim < 4; ++nAim)
            {
                if (isPlayerThere(nAim))
                {
                    attackPlayer();
                }
            }
            break;

        case CLASS_RANDOM_ATTACKER:
            static_cast<CAttacker *>(this)->doManage();
        }
    }
    else
    {
        if (getState(CHitData::STATE_DEAD) && m_propi[lgck::EXTRA_ANIMSEQ] == CGame::INVALID && !po.isPlayer())
        {
            unMap();
            kill();
        }
    }
}

int CActor::childCount()
{
    const CScene &scene = m_game->scene();
    int count = 0;
    for (int i = 0; i < scene.getSize(); ++i)
    {
        CActor &actor = scene[i];
        if (static_cast<unsigned int>(actor.m_propi[lgck::EXTRA_OWNER]) == m_id)
        {
            ++count;
        }
    }
    return count;
}

void CActor::doManage_PlayerBullet()
{
    const CProto &po = proto();
    int ticks = m_game->getTicks();
    bool hasHit = false;
    if (!po.m_nMoveSpeed || (ticks % (po.m_nMoveSpeed) == 0))
    {
        CHitData hitData;
        hitTest(m_nAim, hitData);
        CScene &scene = m_game->scene();
        for (int i = 0; i < hitData.acCount; ++i)
        {
            CActor &tE = scene[hitData.acEntry[i]];
            if (tE.isMonster())
            {
                callEvent(CObject::EO_ATTACK);
                attack(tE);
                hasHit = true;
                break;
            }
        }
        if (!hasHit && canMove(m_nAim))
        {
            unMap();
            move();
            map();
        }
        else
        {
            unMap();
            callEvent(CObject::EO_DEATH);
            kill();
        }
    }
}

void CActor::animate()
{
    if (m_nProto > 0)
    {
        const CProto &p = proto();
        if ((!p.m_nAniSpeed || (m_game->getTickCount() % p.m_nAniSpeed == 0)) && !isFrozen() && m_propi[lgck::EXTRA_ANIMPTR] != CGame::INVALID && m_propi[lgck::EXTRA_ANIMSEQ] != CGame::INVALID)
        {
            CAnimation &animation = object().getAnimation(m_propi[lgck::EXTRA_ANIMSEQ]);
            if (m_propi[lgck::EXTRA_ANIMPTR] >= animation.getSize())
            {
                switch (p.m_nClass)
                {
                case CLASS_ANIMATE_ONCE:
                    callEvent(CObject::EO_DEATH);
                    kill();
                    break;
                }
                if (m_propi[lgck::EXTRA_DEATHINDICATOR])
                {
                    m_propi[lgck::EXTRA_DEATHINDICATOR] = CGame::DI_REMOVAL;
                }
                if (animation.getOptions() & CObject::ASO_REPETE && !getState(CHitData::STATE_DEAD))
                {
                    m_propi[lgck::EXTRA_ANIMPTR] = 0;
                }
                else
                {
                    m_propi[lgck::EXTRA_ANIMSEQ] = CGame::INVALID;
                }
            }
            else
            {
                if (!getState(CHitData::STATE_DEAD))
                {
                    unMap();
                    m_nFrameNo = animation[m_propi[lgck::EXTRA_ANIMPTR]];
                    map();
                }
                else
                {
                    m_nFrameNo = animation[m_propi[lgck::EXTRA_ANIMPTR]];
                }
                ++m_propi[lgck::EXTRA_ANIMPTR];
            }
        }
    }
}

void CActor::crash()
{
    const CProto &protoPlayer = proto();
    CFrame &frame = m_game->toFrame(*this);
    if ((m_nY >= 0) &&
        (m_game->BUFFERHEI - m_nY > frame.hei()))
    {
        Size sx = CMap::size(frame);
        CMap &m = m_game->map();
        Pos p = m.toMap(m_nX, m_nY);
        CScene &scene = m_game->scene();
        for (int x = 0; x < sx.len; ++x)
        {
            const CMapEntry &map = m.at(p.x + x, p.y + sx.hei);
            if (map.m_nAcEntry != 0)
            {
                int spriteId = map.m_nAcEntry;
                CActor &monster = scene[spriteId];
                const CProto &protoMonster = monster.proto();
                if (!CGame::isPlatformClass(protoMonster.m_nClass))
                {
                    if (protoMonster.m_nPowerLevel < protoPlayer.m_nPowerLevel && !protoMonster.getOption(CProto::OPTION_NO_SPLAT))
                    {
                        monster.callEvent(CObject::EO_SPLAT);
                        monster.unMap();
                        if (monster.m_nTriggerKey & TRIGGER_KEYS)
                        {
                            monster.callTrigger();
                        }
                        monster.callEvent(CObject::EO_DEATH);
                        if (!monster.tryAnimation(CObject::AS_DEAD + monster.m_nAim))
                        {
                            monster.stopAnimation();
                        }
                        monster.setState(CHitData::STATE_DEAD, true);
                    }
                    else
                    {
                        if ((protoMonster.m_nPowerLevel > protoPlayer.m_nPowerLevel) && !(monster.m_nTriggerKey & TRIGGER_FROZEN))
                        {
                            monster.attackPlayer();
                        }
                    }
                }
            }
        }
    }
}

void CActor::setTag(int flag, bool bit)
{
    if (bit)
    {
        m_propi[lgck::EXTRA_TAGFLAGS] |= flag;
    }
    else
    {
        m_propi[lgck::EXTRA_TAGFLAGS] -= m_propi[lgck::EXTRA_TAGFLAGS] & flag;
    }
}

bool CActor::isTagged(int flag)
{
    return (m_propi[lgck::EXTRA_TAGFLAGS] & flag) != 0;
}

int CActor::getSeed()
{
    return m_seed;
}

bool CActor::isActive()
{
    return isTagged(CActor::TAG_ACTIVATED);
}

void CActor::activate()
{
    setTag(CActor::TAG_ACTIVATED, true);
}

void CActor::deactivate()
{
    setTag(CActor::TAG_ACTIVATED, false);
}

void CActor::autoCenter(int nAim)
{
    CGame &game = *m_game;
    int granular = isVisible() || m_nY < 0 ? 1 : 8;
    switch (nAim)
    {
    case CGame::UP:
        if ((m_nY - game._my()) <= game.m_screenHei / 2)
        {
            if (game._my() != 0)
            {
                game._my() -= granular;
                if (game._my() < 0)
                {
                    game._my() = 0;
                }
            }
        }
        break;

    case CGame::DOWN:
        if ((m_nY - game._my()) >= game.m_screenHei / 2)
        {
            if (game._my() + game.m_screenHei < game.BUFFERHEI)
            {
                game._my() += granular;
            }
        }
        break;

    case CGame::LEFT:
        if ((m_nX - game._mx()) <= game.m_screenLen / 2)
        {
            if (game._mx() != 0)
            {
                game._mx() -= granular;
            }
        }
        break;

    case CGame::RIGHT:
        if ((m_nX - game._mx()) >= game.m_screenLen / 2)
        {
            if (game._mx() + game.m_screenLen < game.BUFFERLEN)
            {
                game._mx() += granular;
            }
        }
    }
}

void CActor::autoCenter()
{
    int speed = m_game->var("AUTO_CENTER_SPEED");
    if (m_nX < m_game->_mx())
    {
        autoCenter(CGame::LEFT);
        autoCenter(CGame::LEFT);
    }

    if (m_nX > m_game->_mx() + m_game->m_screenLen)
    {
        autoCenter(CGame::RIGHT);
        autoCenter(CGame::RIGHT);
    }

    if (m_nY < m_game->_my() + (20 * m_game->m_screenHei) / 100)
    {
        autoCenter(CGame::UP);
        autoCenter(CGame::UP);
    }

    if (m_nY > m_game->_my() + (80 * m_game->m_screenHei) / 100)
    {
        autoCenter(CGame::DOWN);
        autoCenter(CGame::DOWN);
    }

    // fix a situation when the player left the view
    int rx = m_nX - m_game->_mx();
    if (rx < m_game->m_screenLen / 8 && m_game->_mx() > 0)
    {
        autoCenter(CGame::LEFT);
        rx = m_nX - m_game->_mx();
    }

    if ((rx > m_game->m_screenLen - m_game->m_screenLen / 8) && (m_game->_mx() < m_game->BUFFERLEN - m_game->m_screenLen))
    {
        autoCenter(CGame::RIGHT);
        rx = m_nX - m_game->_mx();
    }

    if (!getState(CHitData::STATE_LOOKUP) &&
        (!speed || m_game->getTicks() % speed == 0))
    {
        if (m_propi[lgck::EXTRA_PATHPTR] == CGame::INVALID)
        {
            autoCenter(CGame::UP);
            autoCenter(CGame::DOWN);
        }
        autoCenter(CGame::LEFT);
        autoCenter(CGame::RIGHT);
    }
}

void CActor::setOwner(int ownerEntry)
{
    // TODO: change ownerId from id to uid
    m_propi[lgck::EXTRA_OWNER] = ownerEntry;
}

bool CActor::isSolid(const CMapEntry *pMap, int aim)
{
    const CMapEntry &map = (*pMap);
    const CProto &protoActor = proto();
    const CScene &scene = m_game->scene();
    if ((map.m_nAcClass >= CLASS_GENERIC_COS))
    {
        return true;
    }
    if (map.m_nBkClass == CLASS_SOLID_BACKGROUND ||
        ((aim == CGame::DOWN || aim == CGame::FALL) &&
         (map.m_nBkClass == CLASS_SOLID__AIM_DOWN_ONLY_)))
    {
        return true;
    }
    // enfore nosplat for player
    if (protoActor.m_nClass == CLASS_PLAYER_OBJECT &&
        aim == CGame::FALL && map.m_fwCount)
    {
        for (int i = 0; i < map.m_fwCount; ++i)
        {
            const CActor &entry = scene[map.m_nFwEntry[i]];
            const CProto &protoMap = m_game->m_arrProto[entry.m_nProto];
            if (protoMap.getOption(CProto::OPTION_NO_SPLAT))
            {
                return true;
            }
        }
    }
    for (int i = 0; i < map.m_fwCount; ++i)
    {
        // enforce solid to monster
        if (!protoActor.isPlayer())
        {
            // if everything else
            const CActor &entry = scene[map.m_nFwEntry[i]];
            const CProto &protoMap = m_game->m_arrProto[entry.m_nProto];
            if (protoMap.getOption(CProto::OPTION_SOLID))
            {
                return true;
            }
        }
        if (map.m_nFwClass[i] == CLASS_OPEN_TO_OWNER)
        {
            if (!protoActor.isPlayer() || aim == CGame::FALL)
            {
                return true;
            }
            else
            {
                CActor &object = scene[map.m_nFwEntry[i]];
                CProto &protoObject = m_game->m_arrProto[object.m_nProto];
                CInventory *inventory = m_game->getInventory();
                if (!protoObject.m_nProtoBuddy || !inventory || !inventory->hasItem(protoObject.m_nProtoBuddy) || protoObject.isStateSolid(aim))
                {
                    return true;
                }
            }
        }
        CActor &object = scene[map.m_nFwEntry[i]];
        CProto &protoObject = m_game->m_arrProto[object.m_nProto];
        if (protoObject.m_solidState & CProto::SOLID_OVERIDE)
        {
            if (protoObject.isStateSolid(aim))
            {
                return true;
            }
        }
        else
        {
            if ((map.m_nFwClass[i] == CLASS_SOLID_ANIMATED) || (map.m_nFwClass[i] == CLASS_TELEPORT_PAD) || (map.m_nFwClass[i] == CLASS_TELEPORT_DESTINATION))
            {
                return true;
            }
        }
    }
    return false;
}

bool CActor::isAntiGravity(const CMapEntry *pMap)
{
    const CMapEntry &map = (*pMap);
    const CScene &scene = m_game->scene();
    if (map.m_nBkClass == CLASS_ANTIGRAVITY ||
        map.m_nBkClass == CLASS_METAL_LADDERS)
    {
        return true;
    }
    else
    {
        if (map.m_nAcEntry)
        {
            const CActor &actor = scene[map.m_nAcEntry];
            const CProto &proto = m_game->m_arrProto[actor.m_nProto];
            if (proto.getOption(CProto::OPTION_NO_GRAVITY))
            {
                return true;
            }
        }
        for (int i = 0; i < map.m_fwCount; ++i)
        {
            const CActor &entry = scene[map.m_nFwEntry[i]];
            const CProto &proto = m_game->m_arrProto[entry.m_nProto];
            if (proto.getOption(CProto::OPTION_NO_GRAVITY))
            {
                return true;
            }
        }
    }
    return false;
}

int CActor::checkHit()
{
    if (m_nY < 0)
    {
        return 0;
    }
    CInventory *inventory = getInventory();
    CHitData data;
    hitTest(CGame::DOWN, data);
    CScene &scene = m_game->scene();
    for (int i = 0; i < data.fwCount; ++i)
    {
        CActor &object = scene[data.fwEntry[i]];
        int triggerKey = object.m_nTriggerKey & TRIGGER_KEYS;
        if (data.fwClass[i] == CLASS_TELEPORT_PAD && !object.isFrozen() && triggerKey)
        {
            for (int j = 0; j < scene.getSize(); ++j)
            {
                CActor &temp = scene[j];
                if ((temp.m_nTriggerKey & TRIGGER_KEYS) == triggerKey)
                {
                    CProto &proto = m_game->m_arrProto[temp.m_nProto];
                    if (proto.m_nClass == CLASS_TELEPORT_DESTINATION)
                    {
                        CFrame &frame = m_game->toFrame(*this);
                        unMap();
                        moveTo(temp.m_nX + 8, temp.m_nY - frame.hei());
                        map();
                        data.flags |= CHitData::FLAG_TELEPORT;
                    }
                }
            }
        }
    }
    memset(static_cast<void *>(&data), 0, sizeof(CHitData));
    hitTest(CGame::HERE, data);
    for (int i = 0; i < data.fwCount; ++i)
    {
        CActor &object = scene.get(data.fwEntry[i]);
        const CProto &protoObj = m_game->m_arrProto[object.m_nProto];
        int triggerKey = object.m_nTriggerKey & TRIGGER_KEYS;
        if (object.m_nProto > 0)
        {
            object.callEvent(CObject::EO_TOUCH);
        }
        switch (data.fwClass[i])
        {
        case CLASS_OPEN_TO_OWNER:
            if (protoObj.getOption(CProto::OPTION_DESTROY_ON_USE))
            {
                object.callEvent(CObject::EO_DEATH);
                object.unMap();
                object.kill();
                if (protoObj.m_nProtoBuddy && inventory)
                {
                    inventory->removeItem(protoObj.m_nProtoBuddy);
                }
            }
            break;
        case CLASS_INVENTORY_ITEM:
            if (inventory)
            {
                inventory->addItem(object.m_nProto);
            }
            /* fall through */
        case CLASS_PICKUP_TRIGGERS:
            object.unMap();
            object.callEvent(CObject::EO_DEATH);
            if (triggerKey)
            {
                object.callTrigger();
            }
            object.kill();
        }
    }
    return data.flags;
}

bool CActor::isFrozen()
{
    return (m_nTriggerKey & TRIGGER_FROZEN) != 0;
}

bool CActor::isGoal()
{
    return (m_nTriggerKey & TRIGGER_GOAL) != 0;
}

bool CActor::isPickUpTrigger()
{
    return proto().m_nClass == CLASS_PICKUP_TRIGGERS;
}

int CActor::getHP()
{
    return m_propi[lgck::EXTRA_HP];
}

void CActor::setHP(int hp)
{
    m_propi[lgck::EXTRA_HP] = hp;
}

int CActor::getFlags()
{
    return m_propi[lgck::EXTRA_FLAGS];
}

unsigned int CActor::getAge()
{
    return m_propi[lgck::EXTRA_AGE];
}

void CActor::getExtra(int *&extra, int *&index, int &size)
{
    size = lgck::EXTRA_COUNT;
    extra = new int[size];
    index = new int[size];
    for (int i = 0; i < size; ++i)
    {
        extra[i] = m_propi[i];
        index[i] = i;
    }
}

const char *CActor::getInventoryName()
{
    static char name[16];
    if (proto().isPlayer())
    {
        strncpy(name, "@player", sizeof(name));
    }
    else
    {
        sprintf(name, "@%.8x", m_seed);
    }
    return name;
}

CInventory *CActor::getInventory()
{
    return m_game->getInventory(getInventoryName());
}

void CActor::resetInventory()
{
    CInventory *inv = getInventory();
    if (inv)
    {
        inv->reset();
    }
}

void CActor::addItem(int protoId, int count)
{
    CInventory *inv = getInventory();
    if (inv)
    {
        inv->addItem(protoId, count);
    }
}

void CActor::removeItem(int protoId)
{
    CInventory *inv = getInventory();
    if (inv)
    {
        inv->removeItem(protoId);
    }
}

bool CActor::hasItem(int protoId)
{
    CInventory *inv = getInventory();
    return inv ? inv->hasItem(protoId) : false;
}

void CActor::set(const char *name, int value)
{
    m_props[name] = value;
}

int &CActor::get(const char *name)
{
    return m_props[name];
}

void CActor::set(int i, int value)
{
    m_propi[i] = value;
}

int &CActor::get(int i)
{
    return m_propi[i];
}

bool CActor::isPlayer()
{
    return proto().isPlayer();
}

void CActor::togglePathPlayback(bool enable)
{
    if (enable)
    {
        m_playback |= CPathBlock::PB_PLAYBACK;
    }
    else
    {
        m_playback &= (-1 ^ CPathBlock::PB_PLAYBACK);
    }
    if (m_path && (m_playback & CPathBlock::PB_PLAYBACK) && tryPath(CObject::PS_SPRITE_CUSTOM))
    {
        m_propi[lgck::EXTRA_PATHDIR] = CObject::PS_SPRITE_CUSTOM;
    }
    else
    {
        m_propi[lgck::EXTRA_PATHDIR] = CGame::INVALID;
        m_propi[lgck::EXTRA_PATHPTR] = 0;
    }
}

const char *CActor::getClassName()
{
    return m_game->m_className[proto().m_nClass].c_str();
}

const char *CActor::getName()
{
    return getProto().getName();
}

const char *CActor::getString()
{
    return m_game->getStringTable()->get(m_string);
}

void CActor::debug()
{
    printf("id: %d [proto %d - %s] class: %d [%s]",
           this->m_id,
           this->m_nProto,
           getProto().getName(),
           getProto().m_nClass,
           getClassName());
}
