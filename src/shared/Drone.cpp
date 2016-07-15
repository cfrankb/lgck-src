#include "stdafx.h"
#include "Game.h"
#include "Drone.h"
#include "Extra.h"
#include "vlamits3.h"

CDrone::CDrone(): CMonster()
{
}

void CDrone::doManage()
{
    CGame & game = * (CGame *) m_game;
    CProto & proto = game.m_arrProto[m_nProto];

    int nAim = m_nAim;
    int ticks = game.getTicks();

    switch(proto.m_nClass)
    {
    case CLASS_DRONE_LEFT_RIGHT: //////////////////////////////////////
        unMap();
        if (nAim == CGame::UP || nAim == CGame::DOWN) {
            nAim += CGame::LEFT;
        }

    case CLASS_DRONE_UP_DOWN: /////////////////////////////////////////
        unMap();
        if (canFall () ) {
            if ( !proto.m_nFallSpeed || ticks % proto.m_nFallSpeed == 0 ) {
                moveBy(0,8);
                if (!m_propi[EXTRA_FALLHEIGHT]) {
                    callEvent(CObject::EO_FALL);
                }

                ++m_propi[EXTRA_FALLHEIGHT];
            }
        }
        else {

            // TODO: implement MaxFall
            if (m_propi[EXTRA_FALLHEIGHT]) {
                land();
            }
            m_propi[EXTRA_FALLHEIGHT] = 0;

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

        if (m_propi[EXTRA_ANIMSEQ] == -1 || CObject::AS_MOVE + nAim != m_propi[EXTRA_ANIMSEQ]) {
            tryAnimation(CObject::AS_MOVE + nAim);
        }

        break; // CLASS_DRONE_UP_DOWN && LEFT_RIGHT
    }

}
