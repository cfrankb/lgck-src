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

#include "stdafx.h"
#include "Scene.h"
#include "Game.h"
#include "vlamits3.h"
#include "Map.h"
#include "Snd.h"
#include "Frame.h"
#include "../shared/Layer.h"
#include "IFile.h"
#include "Extra.h"
#include <cstring>

char CScene::m_signature[] = "SCNX";

CScene::CScene()
{
    m_size = 0;
    m_max = GROWBY;
    m_actors = new CActor *[m_max];
    m_seed = 0;
}

CScene::~CScene()
{
    if (m_actors) {
        forget();
        delete [] m_actors;
    }
}

int CScene::add(const CLevelEntry & entry, bool toSpawn)
{
    if (m_size == m_max) {
        m_max += GROWBY;
        CActor ** t = new CActor* [ m_max ];
        for (int i = 0; i < m_size; ++i) {
            t[i] = m_actors[i];
        }
        delete [] m_actors;
        m_actors = t;
    }
    CActor * t = new CActor(entry);
    t->m_id = m_size;
    t->m_seed = m_seed++;
    m_actors[m_size] = t;
    ++m_size;
    if (toSpawn) {
        t->spawn();
    }
    return t->m_id;
}

int CScene::add(CActor & actor, bool toSpawn)
{   
    bool bFound = false;
    int n;
    for (n = 1; n < m_size && !bFound; ++n) {
        bFound = m_actors[n]->m_nProto == 0;
    }
    if (!bFound) {
        n = m_size;
        if (m_size==m_max) {
            m_max += GROWBY;
            CActor ** t = new CActor* [ m_max ];
            for (int i=0; i < m_size; ++i) {
                t[i] = m_actors[i];
            }
            delete [] m_actors;
            m_actors = t;
        }
        m_actors[n] = new CActor;
        ++m_size;
    } else {
        --n;
    }
    CActor &t = *(m_actors[n]);
    t = actor;
    t.m_id = n;
    t.m_seed = m_seed++;
    if (toSpawn) {
        t.spawn();
    }
    return n;
}

void CScene::removeAt(int i)
{
    for (int j=i; j < m_size -1; ++j) {
        m_actors[j] = m_actors[j + 1];
    }
    --m_size;
}

void CScene::forget()
{
    for (int i=0; i < m_size; ++i) {
        delete m_actors[i];
    }
    m_size = 0;
}

CActor & CScene::operator [] (int i) const
{
    return *(m_actors [ i ]);
}

CLevelEntry & CScene::atIndex (int i) const
{
    return *(dynamic_cast<CLevelEntry*>(m_actors [ i ]));
}

void CScene::setOwner(CGame *game, bool bk)
{
    m_game = game;
    m_bk = bk;
}

int CScene::getSize() const
{
    return m_size;
}

int CScene::findPlayerEntry()
{
    for (int i = 0; i < m_size; ++i) {
        if (m_actors [ i ]->proto().m_nClass == CLASS_PLAYER_OBJECT) {
            return i;
        }
    }

    return -1;
}

void CScene::hideSystemObject (bool bHide)
{
    for (int n = 0; n < m_size; ++n) {
        CActor & actor = *(m_actors[n]);
        if (actor.proto().getOption(CProto::OPTION_INVISIBLE)) {
            if ( bHide ) {
                actor.m_nFrameNo |= 0x8000;
            } else {
                actor.m_nFrameNo &= 0x7fff;
            }
        }
    }
}

int CScene::countGoals()
{
    int goals = 0;
    for (int i=0; i < m_size; ++i) {
        if (m_actors[i]->isGoal()) {
            ++goals;
        }
    }
    return goals;
}

void CScene::animate()
{
    for (int i=0; i < m_size; ++i) {
        m_actors[i]->animate();
    }
}

void CScene::doManage()
{
    for (int n = 0; n < m_size; ++n) {
        m_actors[n]->doManage();
    } // for n
}

int CScene::whoIs(int x, int y)
{
    CGame & game = *m_game;
    int target = -1;
    for (int n = 0; n < m_size; ++n) {
        CActor &entry = *(m_actors[n]);
        CFrame & frame = game.toFrame(entry);
        if (entry.m_nProto && (entry.m_nX<= x) && (entry.m_nY<= y) &&
                (entry.m_nX+ frame.m_nLen) > x &&
                (entry.m_nY+ frame.m_nHei) > y ) {
            target = n;
        }
    }
    return target;
}

const CScene & CScene::operator = (const CLayer & layer)
{
    forget();
    for (int i = 0; i < layer.getSize(); ++i) {
        add(CActor(layer[i]));
    }
    return *this;
}

void CScene::map()
{
    CGame & game = *m_game;
    CMap & rmap = (*game.m_map);
    for (int n = 0; n < m_size; ++ n) {
        CActor & entry = *(m_actors [n]);
        CFrame & frame = game.toFrame(entry);
        const CProto & proto = game.m_arrProto[entry.m_nProto];
        if (proto.m_nClass) {
            if (m_bk) {
                Size sx = rmap.size(& frame);
                Pos p = rmap.toMap(entry.m_nX, entry.m_nY);
                for (int y = 0; y < sx.hei; ++y) {
                    for (int x = 0; x < sx.len; ++ x) {
                        CMapEntry & map = rmap.at(p.x + x, p.y + y);
                        if ( proto.m_bNoSmartMap || frame.map(x,y) ) {
                            map.m_nBkClass = proto.m_nClass;
                        }
                    }
                }
            } else {
                entry.map();
            }
        }
    }
}

int CScene::findBySeed(uint32_t seed)
{
    for (int n = 0; n < m_size; ++ n) {
        if ( (m_actors[n]->m_seed == seed)
                && (m_actors[n]->proto() != 0)) {
            return n;
        }
    }
    return -1;
}

void CScene::notifyClosure()
{
    notifyAll(CObject::EO_NOTIFYCLOSURE);
}

void CScene::notifyAll(int eventId)
{
    for (int i=0; i < m_size; ++i) {
        CActor & entry = *(m_actors[i]);
        if (entry.m_nProto > 0) {
            entry.callEvent(eventId);
        }
    }
}

void CScene::manageAuto()
{
    CGame & game = *m_game;
    int nRateFactor;
    int ticks = game.getTickCount();
    for (int i=0; i < m_size; ++i) {
        CActor & entry = *(m_actors[i]);
        if ( (!(entry.m_nTriggerKey & TRIGGER_FROZEN))
                && entry.isActive()) {
            const CProto & proto = game.m_arrProto[entry.m_nProto];
            CFrame & frame = game.toFrame(entry);
            int nTicksT = ticks + i * 20 + (i % 20);
            bool autoEvent = false;
            // AutoFire
            if (entry.isVisible()) {
                nRateFactor = proto.m_nAutoBulletTime *20 ;
                if ((proto.m_nAutoBullet!=0) &&
                    (nRateFactor &&
                     (nTicksT % nRateFactor)==0)) {

                    entry.callEvent(CObject::EO_FIRE);
                    autoEvent = true;

                    CProto bulletProto = game.m_arrProto[proto.m_nAutoBullet];
                    CActor bullet(bulletProto);
                    bullet.m_nProto = proto.m_nAutoBullet;
                    bullet.m_nAim = entry.m_nAim;
                    bullet.m_nX = entry.m_nX;
                    bullet.m_nY = entry.m_nY;

                    // TODO: check this again

                    switch (bullet.m_nAim) {
                    case CGame::UP:
                    case CGame::LEFT:
                        if (bullet.canMove(bullet.m_nAim)) {
                            bullet.move();
                        }
                        break;

                    case CGame::RIGHT:
                        bullet.m_nX += frame.m_nLen;
                        bullet.move();
                        break;

                    case CGame::DOWN:
                        bullet.m_nY += frame.m_nHei;
                        bullet.move();
                        break;

                    }
                    add(bullet);
                }

                // ChangeTo
                nRateFactor = proto.m_nAutoProtoTime *20 ;
                if (nRateFactor && (nTicksT % nRateFactor)==0) {
                    CProto newProto = game.m_arrProto[proto.m_nAutoProto];
                    autoEvent = true;
                    entry.unMap();
                    entry.m_nFrameSet = newProto.m_nFrameSet;
                    entry.m_nFrameNo = newProto.m_nFrameNo;
                    entry.m_nProto = proto.m_nAutoProto;

                    if (!entry.tryAnimation(CObject::AS_DEFAULT)) {
                        entry.set(EXTRA_ANIMPTR, 0);
                        entry.set(EXTRA_ANIMSEQ, -1);
                    }

                    entry.map();
                }

                // AutoSound
                nRateFactor = proto.m_nAutoSoundTime *20 ;
                if 	(nRateFactor &&
                         (nTicksT % nRateFactor)==0)  {
                    autoEvent = true;
                    int nSound = proto.m_nAutoSound;
                    if (nSound) {
                        game.playSound(nSound);
                    }
                }

            } // if IsVisible

            // AutoTrigger
            if (proto.m_bAutoTrigger) {
                nRateFactor = proto.m_nAutoTriggerTime *20 ;
                if (nRateFactor &&
                         (nTicksT % nRateFactor)==0) {
                    autoEvent = true;
                    entry.callTrigger();
                }
            }

            if (autoEvent) {
                entry.callEvent(CObject::EO_AUTO);
            }
        }
    }
}

void CScene::read(IFile & file)
{
    int version = 0;
    int size = 0;
    char signature[strlen(m_signature)];
    file.read(signature, strlen(m_signature));
    ASSERT(!memcmp(signature, m_signature, strlen(m_signature)));
    file.read(&version,4);
    ASSERT(version==VERSION);
    file.read(&size, 4);
    forget();
    file.read(&m_seed, sizeof(m_seed));
    file.read(&m_bk, sizeof(m_bk));
    delete [] m_actors;
    m_actors = new CActor*[size];
    m_size = size;
    m_max = size;
    for (int i=0; i < size; ++i) {
        m_actors[i] = new CActor();
        m_actors[i]->read(file,version);
    }
}

void CScene::write(IFile &file)
{
    int version = VERSION;
    file.write(m_signature, strlen(m_signature));
    file.write(&version,4);
    file.write(&m_size, 4);
    file.write(&m_seed, sizeof(m_seed));
    file.write(&m_bk, sizeof(m_bk));
    for (int i=0; i < m_size; ++i) {
        CActor * actor = m_actors[i];
        actor->write(file);
    }
}

const CScene &CScene::operator =(const CScene &s)
{
    qDebug("const CScene &CScene::operator =(const CScene &s)");
    forget();
    delete [] m_actors;
    m_size = s.m_size;
    m_max = m_size;
    m_actors = new CActor*[m_size];
    for (int i=0; i < m_size; ++i) {
        m_actors[i] = new CActor();
        *m_actors[i] = s[i];
    }
    m_bk = s.m_bk;
    m_game = s.m_game;
    m_seed = s.m_seed;
    return *this;
}
