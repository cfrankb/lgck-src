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
#include "Game.h"
#include "Drone.h"
#include "Extra.h"
#include "vlamits3.h"

CDrone::CDrone(): CMonster()
{
}

void CDrone::doManage()
{
    CGame & game = *m_game;
    CProto & proto = game.toProto(m_nProto);

    int nAim = m_nAim;
    int ticks = game.getTicks();

    switch(proto.m_nClass) {
    case CLASS_DRONE_LEFT_RIGHT: //////////////////////////////////////
        unMap();
        if (nAim == CGame::UP || nAim == CGame::DOWN) {
            nAim += CGame::LEFT;
        }
        /* fall through */

    case CLASS_DRONE_UP_DOWN: /////////////////////////////////////////
        unMap();
        if (canFall () ) {
            if ( !proto.m_nFallSpeed || ticks % proto.m_nFallSpeed == 0 ) {
                moveBy(0,8);
                if (!m_propi[lgck::EXTRA_FALLHEIGHT]) {
                    callEvent(CObject::EO_FALL);
                }
                ++m_propi[lgck::EXTRA_FALLHEIGHT];
            }
        } else {
            // TODO: implement MaxFall
            if (m_propi[lgck::EXTRA_FALLHEIGHT]) {
                land();
            }
            m_propi[lgck::EXTRA_FALLHEIGHT] = 0;
            if ((proto.m_nMoveSpeed==0)|| (ticks % (proto.m_nMoveSpeed) == 0))  {
                if ( (!game.gravity() && canMove (nAim)) ||
                    testAim (nAim, m_nAim)) {
                    if (isPlayerThere(nAim & CGame::RIGHT)) {
                        //&& !getState(player, STATE_HIT)) {
                        attackPlayer();
                    }
                    move(nAim);
                    m_nAim = nAim;
                } else {
                    nAim ^= 1;
                    m_nAim = nAim;
                }
            }
        }
        map();

        if (m_propi[lgck::EXTRA_ANIMSEQ] == -1 || CObject::AS_MOVE + nAim != m_propi[lgck::EXTRA_ANIMSEQ]) {
            tryAnimation(CObject::AS_MOVE + nAim);
        }
        break; // CLASS_DRONE_UP_DOWN && LEFT_RIGHT
    }
}
