#include "stdafx.h"
#include "Game.h"
#include "Attacker.h"
#include "vlamits3.h"
#include "Snd.h"
#include "Frame.h"
#include "Extra.h"
#include  <stdlib.h>
#include  <stdio.h>
#include  <time.h>

CAttacker::CAttacker():CMonster()
{

}

void CAttacker::doManage()
{
    CGame & game = * (CGame *) m_game;
    CProto & proto = game.m_arrProto[m_nProto];
    CFrame *frame = game.m_arrFrames.getFrame( *this );
    CActor & player = game.getPlayer();
    CFrame *playerFrame = game.m_arrFrames.getFrame(player);
    CFrame *monsterFrame = game.m_arrFrames.getFrame(*this);
    int ticks = game.getTicks();

    unMap();

    if (m_propi[EXTRA_ACTIVE]) {
        if (m_propi[EXTRA_TIMEOUT]) {
            --m_propi[EXTRA_TIMEOUT];
        } else {
            switch (m_propi[EXTRA_ACTIVE]) {

            case 1:

                m_nAim = ::rand() & CGame::AIM_MASK;
                tryAnimation(CObject::AS_MOVE + m_nAim);

                switch ( m_nAim ) {
                case CGame::UP:
                    m_nY = game.BUFFERHEI - monsterFrame->m_nHei;
                    m_nX = (::rand () % game.BUFFERLEN) & 0x7ff0;
                    break;

                case CGame::DOWN:
                    m_nY = 0;
                    m_nX = (::rand () % game.BUFFERLEN) & 0x7ff0;
                    break;

                case CGame::LEFT:
                    m_nX = game.BUFFERLEN - monsterFrame->m_nLen;
                    m_nY = (::rand () % game.BUFFERHEI) & 0x7ff0;
                    break;

                case CGame::RIGHT:
                    m_nX = 0;
                    m_nY = (::rand () % game.BUFFERHEI) & 0x7ff0;
                    break;
                }

                ++m_propi[EXTRA_ACTIVE];

                m_nFrameSet = proto.m_nFrameSet;
                m_nFrameNo = proto.m_nFrameNo;

                break;

            case 2:
            case 3:

                int speed = proto.m_nMoveSpeed;
                if (m_propi[EXTRA_ACTIVE] == 3) {
                    speed /= 2;
                }

                if (!speed || (ticks % speed == 0)) {
                    // time to move

                    if (isPlayerThere(m_nAim)) {
                        attackPlayer();
                    }

                    if (m_propi[EXTRA_ACTIVE] == 2
                            && (m_nAim == CGame::LEFT || m_nAim == CGame::RIGHT)) {
                        if ( ((player.m_nX <= m_nX) && (player.m_nX + playerFrame->m_nLen > m_nX))
                             || ((player.m_nX > m_nX) && (player.m_nX < m_nX + monsterFrame->m_nLen))) {

                            ++m_propi[EXTRA_ACTIVE];

                            if ( player.m_nY < m_nY ) {
                                m_nAim = CGame::UP;
                            } else {
                                m_nAim = CGame::DOWN;
                            }
                            //game.m_arrSounds["pow.ogg"]->play();
                            callEvent(CObject::EO_ATTACK);
                        }
                    }

                    if (m_propi[EXTRA_ACTIVE] == 2
                            && (m_nAim == CGame::UP || m_nAim == CGame::DOWN)) {

                        if ( ((player.m_nY <= m_nY) && (player.m_nY + playerFrame->m_nHei > m_nY))
                             || ((player.m_nY > m_nY) && (player.m_nY < m_nY + monsterFrame->m_nHei))) {

                            ++m_propi[EXTRA_ACTIVE];

                            if ( player.m_nX < m_nX ) {
                                m_nAim = CGame::LEFT;
                            } else {
                                m_nAim = CGame::RIGHT;
                            }
                            callEvent(CObject::EO_ATTACK);
                            // game.m_arrSounds["pow.ogg"]->play();
                        }
                    }
                    switch ( m_nAim ) {
                    case CGame::UP:
                        m_nY -= 8;
                        if (m_nY < 0) {
                            m_propi[EXTRA_ACTIVE] = 0;
                        }
                        break;

                    case CGame::DOWN:
                        m_nY += 8;
                        if (m_nY + frame->m_nHei > game.BUFFERHEI) {
                            m_propi[EXTRA_ACTIVE] = 0;
                        }
                        break;

                    case CGame::LEFT:
                        m_nX -= 8;
                        if (m_nX < 0) {
                            m_propi[EXTRA_ACTIVE] = 0;
                        }
                        break;

                    case CGame::RIGHT:
                        m_nX += 8;
                        if (m_nX + frame->m_nLen > game.BUFFERLEN) {
                            m_propi[EXTRA_ACTIVE] = 0;
                        }
                        break;
                    }
                }

                break;
            }
        }
    }

    if (!m_propi[EXTRA_ACTIVE]) {
        m_propi[EXTRA_TIMEOUT] = 20 + ::rand() % 240;
        m_propi[EXTRA_ACTIVE] = 1;
        m_nFrameSet = 0;
        m_nFrameNo = 0;
    }

    map();
}
