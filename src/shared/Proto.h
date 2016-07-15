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

// Proto.h : header file
//

#ifndef _PROTO_H
#define _PROTO_H

class IFile;
class QString;

/////////////////////////////////////////////////////////////////////////////
// CProto

class CProto
{
    // Construction
public:
    char* getName();
    CProto (const char *);
    CProto (const CProto & proto);
    CProto();

    // Attributes
public:

    // Operations
public:
    CProto & operator = (CProto proto);
    bool operator == (const CProto proto) const;
    bool operator != (const CProto proto) const;

    // Implementation
public:
    void read(IFile & file, int nSize=0);
    void write(IFile & file);
    ~CProto();

    UINT8 m_nClass;
    UINT8 m_bNoSmartMap;
    UINT8 m_nJumpMode;
    UINT8 m_nMaxFall;           // TBA

    char  m_szName[32];

    INT16 m_nFrameSet;
    INT16 m_nFrameNo;

    UINT8 m_nMoveSpeed;
    UINT8 m_nAniSpeed;
    UINT8 m_nDefaultAim;
    UINT8 m_nPoints;

    INT16 m_nProtoBuddy;
    UINT16 m_options;

    INT16 m_nChProto;
    INT16 m_nChSound;

    UINT8 m_nFallSpeed;       // how fast this object falls
    UINT8 m_nDamages;
    UINT8 m_nBonusHP;         // hp bonus
    UINT8 m_nPowerLevel;

    UINT8 m_nRbDelay;
    UINT8 m_nRebirths;

    INT16 m_nAutoProto;
    INT16 m_nAutoBullet;
    INT16 m_nAutoSound;
    UINT8 m_bAutoTrigger;
    UINT8 m_nHP;            // TBA

    UINT8 m_nAutoProtoTime;
    UINT8 m_nAutoBulletTime;
    UINT8 m_nAutoSoundTime;
    UINT8 m_nAutoTriggerTime;

    UINT8 m_nMaxBullets;     // max actve bullets
    UINT8 m_nFireRate;   
    UINT8 m_rebirthLocation;
    UINT8 m_solidState;     // flag see SOLID_XXXX

    UINT8 m_extra[2];
    UINT16 m_bulletSound;

    enum {
        OPTION_NO_GRAVITY       = 0x0001,  // antigravity flag
        OPTION_DESTROY_ON_USE   = 0x0002,
        OPTION_SOLID            = 0x0004,  // Solid to Monsters
        OPTION_INVISIBLE        = 0x0008,  // old system invisible
        OPTION_AUTO_GOAL        = 0x0010,  // set the goal flag of new sprites
        OPTION_INACTIVE         = 0x0020,  // doesnt interract until flag is cleared
        OPTION_INACTIVE_USERDEF = 0x0040,  // userdef is activated through scripting
        OPTION_INVENTORY_HIDDEN = 0x0080,  // doesn't display this item in the inventory
        OPTION_ADVANCED         = 0x1000,  // handle advanced stuff like inventory and state
        OPTION_NO_SPLAT         = 0x2000,  // this object cannot be crushed
        OPTION_NO_TRIGGER       = 0x4000,  // don't trigger this object
        OPTION_NO_SHIFT_AT_DEATH= 0x8000   // leave corps behind
    };

    enum {
        SOLID_UP                = 0x01,
        SOLID_DOWN              = 0x02,
        SOLID_LEFT              = 0x04,
        SOLID_RIGHT             = 0x08,
        SOLID_OVERIDE           = 0x80
    };

    enum {
        REBIRTH_ORIGIN          = 0x00,
        REBIRTH_DEATHBED        = 0x01,
        REBIRTH_RANDOM          = 0x02,
        REBIRTH_RANDOM_TOP      = 0x03
    };

    enum {
        PPARAM_NAME          = 0x01,
        PPARAM_CLASS         = 0x02,
        PPARAM_NO_SMART_MAP  = 0x03 ,
        PPARAM_JUMP_MODE     = 0x04,
        PPARAM_MAX_FALL      = 0x05,

        PPARAM_FRAMESET      = 0x06,
        PPARAM_FRAMENO       = 0x07,

        PPARAM_MOVE_SPEED    = 0x08,
        PPARAM_ANI_SPEED     = 0x09,
        PPARAM_DEFAULT_AIM   = 0x0a,
        PPARAM_POINTS        = 0x0b,

        PPARAM_BUDDY         = 0x0c,
        PPARAM_OPTIONS       = 0x0d,

        PPARAM_CHPROTO       = 0x0e,
        PPARAM_CHSOUND       = 0x0f,

        PPARAM_FALLSPEED     = 0x10,
        PPARAM_DAMAGES       = 0x11,
        PPARAM_BONUS_HP      = 0x12,
        PPARAM_POWERLEVELS   = 0x13,

        PPARAM_RBDELAY       = 0x14,
        PPARAM_REBIRTHS      = 0x15,

        PPARAM_A_PROTO       = 0x16,
        PPARAM_A_BULLET      = 0x17,
        PPARAM_A_SOUND       = 0x18,
        PPARAM_A_TRIGGER     = 0x19,
        PPARAM_HP            = 0x1a,

        PPARAM_A_PROTO_TIME  = 0x1b,
        PPARAM_A_BULLET_TIME = 0x1c,
        PPARAM_A_SOUND_TIME  = 0x1d,
        PPARAM_A_TRIGGER_TIME= 0x1e,

        PPARAM_MAX_BULLETS   = 0x1f,
        PPARAM_FIRE_RATES    = 0x20,
        PPARAM_EXTRA1        = 0x21,
        PPARAM_EXTRA2        = 0x22,
        PPARAM_B_SOUND       = 0x23
    };

    bool getOption(int option) const;
    void setOption(int option, bool set);
    bool isStateSolid(int fromAim) const;

    bool isBkClass() const;
    bool isFkClass() const;
    bool isAcClass() const;

};

/////////////////////////////////////////////////////////////////////////////
#endif
