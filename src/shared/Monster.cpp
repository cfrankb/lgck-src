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

#include "stdafx.h"
#include "Monster.h"
#include "Game.h"
#include "Map.h"
#include "Extra.h"
#include <stdlib.h>

CMonster::CMonster():CActor()
{    
}

void CMonster::doManage()
{
    const int aims [4][6] = {
        {CGame::LEFT,   CGame::UP,   CGame::RIGHT, CGame::UP, CGame::UP, CGame::DOWN},
        {CGame::RIGHT,  CGame::DOWN, CGame::LEFT, CGame::DOWN, CGame::DOWN, CGame::UP},
        {CGame::DOWN,  CGame::LEFT,    CGame::UP, CGame::LEFT, CGame::LEAP_LEFT, CGame::RIGHT},
        {CGame::UP, CGame::RIGHT,  CGame::DOWN, CGame::RIGHT, CGame::LEAP_RIGHT, CGame::LEFT}
    };

    CGame & game = * (CGame *) m_game;
    CProto & proto = game.m_arrProto[m_nProto];
    int ticks = game.getTicks();
    int nAim = m_nAim;

    unMap();
    if (m_propi[EXTRA_ANIMSEQ] != CObject::AS_MOVE + nAim) {
        tryAnimation( CObject::AS_MOVE + nAim);
    }

    if (canFall() ) {
        if ( !proto.m_nFallSpeed || ticks % proto.m_nFallSpeed == 0 ) {

            if (!m_propi[EXTRA_FALLHEIGHT]) {
                callEvent(CObject::EO_FALL);
            }

            moveBy(0,8);
            ++m_propi[EXTRA_FALLHEIGHT];
        }
    } else {

        if (m_propi[EXTRA_FALLHEIGHT]) {
            land();
        }

        m_propi[EXTRA_FALLHEIGHT] = 0;
        if ( !getState(CHitData::STATE_DEAD) &&
                ((proto.m_nMoveSpeed == 0) || (ticks % (proto.m_nMoveSpeed) == 0))) {

            if (isPlayerThere(nAim)) {
                attackPlayer();
            }

            bool bcanWalk = false;
            int aim;
            int flipMe = -1;
            for (int x = 0; x < 6 && !bcanWalk; ++x) {
                aim = aims[nAim][x];

                if (aim == flipMe) {
                    aim ^= 1;
                }

                if (!x && (aim ^ 1) == m_propi[EXTRA_OLDAIM2]) {
                    aim = m_propi[EXTRA_OLDAIM2];
                    flipMe = aim;
                }

                bcanWalk = testAim (aim, nAim);
            }

            if (!bcanWalk) {
                bcanWalk = canMove(aim, true);
                if (!bcanWalk) {
                    aim =  ::rand() & CGame::AIM_MASK;
                    bcanWalk = canMove(aim, true);
                    for (int i=0; i < 3 && !bcanWalk; ++i) {
                        aim = (aim + 1) & CGame::AIM_MASK;
                        bcanWalk = canMove(aim, true);
                    }
                }
            }

            if (bcanWalk) {
                move(aim);
                if (aim & CGame::LEAP && canMove ((aim ^ CGame::LEAP), true)) {
                    move(aim ^ CGame::LEAP) ;
                }

                if ((aim & CGame::AIM_MASK) != m_propi[EXTRA_OLDAIM1] ) {
                    m_propi[EXTRA_OLDAIM2] = m_propi[EXTRA_OLDAIM1];
                    m_propi[EXTRA_OLDAIM1] = aim & CGame::AIM_MASK;
                }

                m_nAim = aim & CGame::AIM_MASK;

            } // if bcanWalk

        } // if nTicks
    }
    map();
}

bool CMonster::testAim(int aim, int  oldAim)
{
    CGame & game = * (CGame *) m_game;
    CFrame *frame = game.m_arrFrames.getFrame(*this);
    CMonster t = *this;

    bool leap = aim & CGame::LEAP;
    if ( leap ) {
        if (t.canMove( CGame::UP, true) ) {
            t.move(CGame::UP);
        }
        aim ^= CGame::LEAP;
    }

    if ( t.canMove(aim , true) ) {
        switch ( aim ) {
        case CGame::UP:
        case CGame::DOWN:
        case CGame::LEFT:
        case CGame::RIGHT:
            t.move(aim);
        }

        int support;
        const Size sx = CMap::size(frame);
        int width = sx.len;

        CProto & proto = game.m_arrProto[ m_nProto ];
        bool noGravity = !game.gravity() || proto.getOption(CProto::OPTION_NO_GRAVITY);
        if (noGravity) {
            support = width;
        } else {
            support = t.testSupport();
        }

        if (!leap) {
            if ( ((support < width) || noGravity) && (aim == oldAim) && (aim >= CGame::LEFT) ) {

                for (int i=0; i < support; ++i) {
                    if (t.canMove(aim)) {
                        t.move(aim);
                    } else {
                        return false;
                    }
                }

                if (t.canMove(CGame::DOWN)) {
                    t.move(CGame::DOWN);
                    if (t.testSupport()) {
                        *this = t;
                        return true;
                    }
                }
            }
            return support >= width;
        } else {
            return support;
        }
    }
    return false;
}

int CMonster::testSupport()
{
    CGame & game = * (CGame *) m_game;
    CFrame *frame = game.m_arrFrames.getFrame(*this);
    const Size sx = CMap::size(frame);
    CMap & m = *(game.m_map);
    const Pos p = m.toMap(m_nX, m_nY);
    int support = 0 ;
    for (int x = 0; x < sx.len; ++x) {
        CMapEntry & map = m.at(p.x + x, p.y + sx.hei);
        if ( isSolid(& map, CGame::DOWN) || isAntiGravity(& map)) {
            ++support;
        }
    }

    return support;
}
