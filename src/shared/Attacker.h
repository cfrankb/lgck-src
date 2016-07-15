#ifndef ATTACKER_H
#define ATTACKER_H

#include "Monster.h"

class CAttacker : public CMonster
{
public:
    CAttacker();
    void doManage();
};

#endif // ATTACKER_H
