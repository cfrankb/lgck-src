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
#include "Attacker.h"
#include "Frame.h"
#include "Extra.h"

CAttacker::CAttacker():CMonster()
{

}

void CAttacker::doManage()
{
    CGame & game = *m_game;
    CProto & proto = game.toProto(m_nProto);
    CFrame & frame = game.toFrame( *this );
    CActor & player = game.getPlayer();
    CFrame & playerFrame = game.toFrame( player );
    CFrame & monsterFrame = game.toFrame( *this );
    int ticks = game.getTicks();

    unMap();

    if (m_propi[lgck::EXTRA_ACTIVE]) {
        if (m_propi[lgck::EXTRA_TIMEOUT]) {
            --m_propi[lgck::EXTRA_TIMEOUT];
        } else {
            switch (m_propi[lgck::EXTRA_ACTIVE]) {

            case 1:

                m_nAim = ::rand() & CGame::AIM_MASK;
                tryAnimation(CObject::AS_MOVE + m_nAim);

                switch ( m_nAim ) {
                case CGame::UP:
                    m_nY = game.BUFFERHEI - monsterFrame.hei();
                    m_nX = (::rand () % game.BUFFERLEN) & 0x7ff0;
                    break;

                case CGame::DOWN:
                    m_nY = 0;
                    m_nX = (::rand () % game.BUFFERLEN) & 0x7ff0;
                    break;

                case CGame::LEFT:
                    m_nX = game.BUFFERLEN - monsterFrame.len();
                    m_nY = (::rand () % game.BUFFERHEI) & 0x7ff0;
                    break;

                case CGame::RIGHT:
                    m_nX = 0;
                    m_nY = (::rand () % game.BUFFERHEI) & 0x7ff0;
                    break;
                }

                ++m_propi[lgck::EXTRA_ACTIVE];

                m_nFrameSet = proto.m_nFrameSet;
                m_nFrameNo = proto.m_nFrameNo;

                break;

            case 2:
            case 3:

                int speed = proto.m_nMoveSpeed;
                if (m_propi[lgck::EXTRA_ACTIVE] == 3) {
                    speed /= 2;
                }

                if (!speed || (ticks % speed == 0)) {
                    // time to move

                    if (isPlayerThere(m_nAim)) {
                        attackPlayer();
                    }

                    if (m_propi[lgck::EXTRA_ACTIVE] == 2
                            && (m_nAim == CGame::LEFT || m_nAim == CGame::RIGHT)) {
                        if ( ((player.m_nX <= m_nX) && (player.m_nX + playerFrame.len() > m_nX))
                             || ((player.m_nX > m_nX) && (player.m_nX < m_nX + monsterFrame.len()))) {

                            ++m_propi[lgck::EXTRA_ACTIVE];

                            if ( player.m_nY < m_nY ) {
                                m_nAim = CGame::UP;
                            } else {
                                m_nAim = CGame::DOWN;
                            }
                            callEvent(CObject::EO_ATTACK);
                        }
                    }

                    if (m_propi[lgck::EXTRA_ACTIVE] == 2
                            && (m_nAim == CGame::UP || m_nAim == CGame::DOWN)) {

                        if ( ((player.m_nY <= m_nY) && (player.m_nY + playerFrame.hei() > m_nY))
                             || ((player.m_nY > m_nY) && (player.m_nY < m_nY + monsterFrame.hei()))) {

                            ++m_propi[lgck::EXTRA_ACTIVE];

                            if ( player.m_nX < m_nX ) {
                                m_nAim = CGame::LEFT;
                            } else {
                                m_nAim = CGame::RIGHT;
                            }
                            callEvent(CObject::EO_ATTACK);
                        }
                    }
                    switch ( m_nAim ) {
                    case CGame::UP:
                        m_nY -= 8;
                        if (m_nY < 0) {
                            m_propi[lgck::EXTRA_ACTIVE] = 0;
                        }
                        break;

                    case CGame::DOWN:
                        m_nY += 8;
                        if (m_nY + frame.hei() > game.BUFFERHEI) {
                            m_propi[lgck::EXTRA_ACTIVE] = 0;
                        }
                        break;

                    case CGame::LEFT:
                        m_nX -= 8;
                        if (m_nX < 0) {
                            m_propi[lgck::EXTRA_ACTIVE] = 0;
                        }
                        break;

                    case CGame::RIGHT:
                        m_nX += 8;
                        if (m_nX + frame.len() > game.BUFFERLEN) {
                            m_propi[lgck::EXTRA_ACTIVE] = 0;
                        }
                        break;
                    }
                }

                break;
            }
        }
    }

    if (!m_propi[lgck::EXTRA_ACTIVE]) {
        m_propi[lgck::EXTRA_TIMEOUT] = 20 + ::rand() % 240;
        m_propi[lgck::EXTRA_ACTIVE] = 1;
        m_nFrameSet = 0;
        m_nFrameNo = 0;
    }

    map();
}
