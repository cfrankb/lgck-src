-- common.lua

--[[
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
--]]

----------------------------------------------------------------------
-- SECTION: General
    TRUE		        = 1;
    FALSE 		        = 0;
    NULL 		        = 0;
    INVALID				= -1;

----------------------------------------------------------------------
-- EVENTS

-- SECTION: Sprite Events
    EO_SPAWN   		    = 0;
    EO_ACTIVATE		    = 1;
    EO_DEATH		    = 2;
    EO_HURT	    	    = 3;
    EO_TOUCH		    = 4;
    EO_TRIGGER		    = 5;
    EO_HANDLER		    = 6;
    EO_SPLAT		    = 7;
    EO_HIT_TEST		    = 8;
    EO_ZKEY		        = 9;
    EO_AUTO		        = 10;
    EO_JUMP		        = 11;
    EO_FALL		        = 12;
    EO_LAND		        = 13;
    EO_LEAP		        = 14;
    EO_MOVE		        = 15;
    EO_FIRE		        = 16;
    EO_LEFTCLICK        = 17;
    EO_RIGHTCLICK       = 18;
    EO_NOTIFYCLOSURE    = 19;

-- SECTION: Engine Events
    EVENT_NO_EVENT          = 0;
    EVENT_QUIT              = 1;
    EVENT_LEVEL_COMPLETED   = 2;
    EVENT_TIMEOUT           = 3;
    EVENT_PLAYER_DIED       = 4;
    EVENT_NO_PLAYER         = 5;

-- SECTION: Level Events
    EL_CREATE 			= 0;
    EL_DRAW 			= 1;
    EL_RESTART 			= 2;
    EL_GOAL_COLLECTED 	= 3;
    EL_LEVEL_COMPLETED 	= 4;
    EL_TIMEOUT 			= 5;
    EL_KEY_PRESSED 		= 6;
    EL_KEY_UP 			= 7;
    EL_GOAL_KILLED      = 8;
    EL_HANDLER          = 9;

-- SECTION: Game Events
    EG_INIT_GAME        = 0;
    EG_PREPARE_LEVEL    = 1;
    EG_COMPLETE_LEVEL   = 2;
    EG_DEATH            = 3;
    EG_GOAL_COLLECTED   = 4;
    EG_GOAL_KILLED      = 5;
    EG_PICKUP           = 6;
    EG_LOADGAME         = 7;
    EG_SAVEGAME         = 8;
    EG_RESTART_LEVEL    = 9;
    EG_NOTIFYCLOSURE    = 10;

----------------------------------------------------------------------
-- aims
-- SECTION: Basic Aims
    UP			        = 0x00;
    DOWN		        = 0x01;
    LEFT		        = 0x02;
    RIGHT               = 0x03;
    AIM_MASK            = 0x03;
-- SECTION: Extended Aims
    LEFT_UP	        	= 0x04;
    RIGHT_UP	    	= 0x05;
    LEFT_DOWN	    	= 0x06;
    RIGHT_DOWN	    	= 0x07;
    LEAP                = 0x10;
    LEAP_LEFT	    	= 0x12;
    LEAP_RIGHT	    	= 0x13;

    PAUSE	          	= -2;
    SEPARATOR   		= -1;
    HERE	        	= 255;

----------------------------------------------------------------------
-- SECTION: Joy States
    JOY_UP	    	    = 1;
    JOY_DOWN    		= 2;
    JOY_LEFT	    	= 4;
    JOY_RIGHT	    	= 8;
    JOY_JUMP    		= 16;
    JOY_FIRE	    	= 32;
    JOY_ZKEY    		= 64;

----------------------------------------------------------------------
-- SECTION: Flags

    FLAG_NONE		    = 0x00;
    FLAG_METAL		    = 0x01;
    FLAG_NOSOUND        = 0x02;
    FLAG_WATER		    = 0x04;
    FLAG_DEATH		    = 0x08;
    FLAG_HIT		    = 0x10;
    FLAG_TELEPORT       = 0x20;

----------------------------------------------------------------------
-- SECTION: Player States

    STATE_NONE		    = 0x0;
    STATE_FALL		    = 0x1;
    STATE_DEAD		    = 0x2;
    STATE_HIT		    = 0x4;
    STATE_JUMP		    = 0x8;
    STATE_BEGINNING     = 0x10;
    STATE_FIRING        = 0x20;
    STATE_LOOKUP        = 0x40;

-- SECTION: Proto
    PROTO_POINTS        = 0xffff;
    PROTO_DEAD_PLAYER   = 0xfffe;

-----------------------------------------------------------------------
-- SECTION: Death Indicator

    DI_NONE             = 0;
    DI_ANIMATION        = 1;
    DI_REMOVAL          = 2;

----------------------------------------------------------------------
-- SECTION: Level Params

    SPARAM_TITLE        = 0;
    SPARAM_BKCOLOR      = 1;
    SPARAM_HINT         = 2;
    SPARAM_GOAL         = 3;
    SPARAM_TIME         = 4;
    SPARAM_TRIGGER      = 5;
    SPARAM_WRAP         = 6;
    SPARAM_NO_GRAVITY   = 7;

-- SECTION: Level Wrap

    WRAP_UP             = 1;
    WRAP_DOWN           = 2;
    WRAP_LEFT           = 4;
    WRAP_RIGHT          = 8;

-- SECTION: Level Goal

    GOAL_ONLY           = 0;
    GOAL_UP             = 1;
    GOAL_DOWN           = 2;
    GOAL_LEFT           = 3;
    GOAL_RIGHT          = 4;
    GOAL_NO_COMPLETE    = 5;


----------------------------------------------------------------------
-- SECTION: Display Types

    DISPLAY_TIME_LEFT   = 0;
    DISPLAY_MESSAGE     = 1;
    DISPLAY_SCORE       = 2;
    DISPLAY_LIVES       = 3;
    DISPLAY_HP          = 4;
    DISPLAY_DEBUG	    = 5;
    DISPLAY_IMAGE	    = 6;

----------------------------------------------------------------------
-- SECTION: Skills

    SKILL_NORMAL	    = 0;
    SKILL_NIGHTMARE	    = 1;
    SKILL_HELL		    = 2;
    SKILL_INSANE	    = 3;

----------------------------------------------------------------------
-- layers

-- SECTION: Layer types
    LAYER_BK            = 0;
    LAYER_MAIN          = 1;
    LAYER_FW            = 2;

-- SECTION: Layer scroll speed
    SPEED_NOSCROLL      = 0;
    SPEED_WHOLE         = 1;
    SPEED_1_2           = 2;
    SPEED_1_4           = 3;
    SPEED_1_8           = 4;
    SPEED_1_16          = 5;
    SPEED_1_32          = 6;
    SPEED_1_64          = 7;
    SPEED_1_128         = 8;
    SPEED_CUSTOM        = 0x10;

----------------------------------------------------------------------
-- SECTION: Animation Sequences
    AS_DEFAULT      = 0x00;
    AS_IDLE         = 0x01;
    AS_CUSTOM1      = 0x02;
    AS_CUSTOM2      = 0x03;
    AS_STAND        = 0x04;
    AS_STAND_UP     = 0x04;
    AS_STAND_DOWN   = 0x05;
    AS_STAND_LEFT   = 0x06;
    AS_STAND_RIGHT  = 0x07;
    AS_MOVE         = 0x08;
    AS_MOVE_UP      = 0x08;
    AS_MOVE_DOWN    = 0x09;
    AS_MOVE_LEFT    = 0x0a;
    AS_MOVE_RIGHT   = 0x0b;
    AS_JUMP         = 0x0c;
    AS_JUMP_UP      = 0x0c;
    AS_JUMP_DOWN    = 0x0d;
    AS_JUMP_LEFT    = 0x0e;
    AS_JUMP_RIGHT   = 0x0f;
    AS_HURT         = 0x10;
    AS_HURT_UP      = 0x10;
    AS_HURT_DOWN    = 0x11;
    AS_HURT_LEFT    = 0x12;
    AS_HURT_RIGHT   = 0x13;
    AS_ATTACK       = 0x14;
    AS_ATTACK_UP    = 0x14;
    AS_ATTACK_DOWN  = 0x15;
    AS_ATTACK_LEFT  = 0x16;
    AS_ATTACK_RIGHT = 0x17;
    AS_RUN          = 0x18;
    AS_RUN_UP       = 0x18;
    AS_RUN_DOWN     = 0x19;
    AS_RUN_LEFT     = 0x1a;
    AS_RUN_RIGHT    = 0x1b;
    AS_DEAD         = 0x1c;
    AS_DEAD_UP      = 0x1c;
    AS_DEAD_DOWN    = 0x1d;
    AS_DEAD_LEFT    = 0x1e;
    AS_DEAD_RIGHT   = 0x1f;
    AS_MAX_VALUE    = 0x1f;

-- SECTION: Animation Options
    ASO_REPETE      = 0x0001;
    ASO_OPTION2     = 0x0002;

----------------------------------------------------------------------
-- SECTION: Path Sequences
    PS_DEFAULT           = 0x00;
    PS_IDLE              = 0x01;
    PS_CUSTOM1           = 0x02;
    PS_CUSTOM2           = 0x03;
    PS_ATTACK            = 0x04;
    PS_ATTACK_UP         = 0x04;
    PS_ATTACK_DOWN       = 0x05;
    PS_ATTACK_LEFT       = 0x06;
    PS_ATTACK_RIGHT      = 0x07;
    PS_JUMP              = 0x08;
    PS_JUMP_UP           = 0x08;
    PS_JUMP_DOWN         = 0x09;
    PS_JUMP_LEFT         = 0x0a;
    PS_JUMP_RIGHT        = 0x0b;
    PS_JUMP_UP_LEFT      = 0x0c;
    PS_JUMP_UP_RIGHT     = 0x0d;
    PS_JUMP_DN_LEFT      = 0x0e;
    PS_JUMP_DN_RIGHT     = 0x0f;
    PS_MOVE              = 0x10;
    PS_MOVE_UP           = 0x10;
    PS_MOVE_DOWN         = 0x11;
    PS_MOVE_LEFT         = 0x12;
    PS_MOVE_RIGHT        = 0x13;
    PS_MOVE_UP_LEFT      = 0x14;
    PS_MOVE_UP_RIGHT     = 0x15;
    PS_MOVE_DN_LEFT      = 0x16;
    PS_MOVE_DN_RIGHT     = 0x17;
    PS_SPRITE_CUSTOM     = 0x7f;

---------------------------------------------------------------------
-- SECTION: Extra Data

    EXTRA_ORGPROTO       = 0x01;
    EXTRA_ORGX           = 0x02;
    EXTRA_ORGY           = 0x03;
    EXTRA_HP             = 0x04;
    EXTRA_OLDAIM1        = 0x05;
    EXTRA_OLDAIM2        = 0x06;
    EXTRA_FALLHEIGHT     = 0x07;
    EXTRA_LIVES          = 0x08;
    EXTRA_ACTIVE         = 0x09;
    EXTRA_BULLETCOUNT    = 0x0a;
    EXTRA_OWNER          = 0x0b;
    EXTRA_FLAGS          = 0x0c;
    EXTRA_PATHDIR        = 0x0d;
    EXTRA_PATHPTR        = 0x0e;
    EXTRA_ANIMSEQ        = 0x0f;
    EXTRA_ANIMPTR        = 0x10;
    EXTRA_ANIMSPEED      = 0x11;
    EXTRA_DEATHINDICATOR = 0x12;
    EXTRA_AGE            = 0x13;
    EXTRA_TIMEOUT        = 0x14;
    EXTRA_TAGFLAGS       = 0x15;
    EXTRA_INVENTORY      = 0x16;

---------------------------------------------------------------------
-- SECTION: Proto Data

    PPARAM_NAME          = 0x01;
    PPARAM_CLASS         = 0x02;
    PPARAM_NO_SMART_MAP  = 0x03;
    PPARAM_JUMP_MODE     = 0x04;
    PPARAM_MAX_FALL      = 0x05;
    PPARAM_FRAMESET      = 0x06;
    PPARAM_FRAMENO       = 0x07;
    PPARAM_MOVE_SPEED    = 0x08;
    PPARAM_ANI_SPEED     = 0x09;
    PPARAM_DEFAULT_AIM   = 0x0a;
    PPARAM_POINTS        = 0x0b;
    PPARAM_BUDDY         = 0x0c;
    PPARAM_OPTIONS       = 0x0d;
    PPARAM_CHPROTO       = 0x0e;
    PPARAM_CHSOUND       = 0x0f;
    PPARAM_FALLSPEED     = 0x10;
    PPARAM_DAMAGES       = 0x11;
    PPARAM_BONUS_HP      = 0x12;
    PPARAM_POWERLEVELS   = 0x13;
    PPARAM_RBDELAY       = 0x14;
    PPARAM_REBIRTHS      = 0x15;
    PPARAM_A_PROTO       = 0x16;
    PPARAM_A_BULLET      = 0x17;
    PPARAM_A_SOUND       = 0x18;
    PPARAM_A_TRIGGER     = 0x19;
    PPARAM_HP            = 0x1a;
    PPARAM_A_PROTO_TIME  = 0x1b;
    PPARAM_A_BULLET_TIME = 0x1c;
    PPARAM_A_SOUND_TIME  = 0x1d;
    PPARAM_A_TRIGGER_TIME= 0x1e;
    PPARAM_MAX_BULLETS   = 0x1f;
    PPARAM_FIRE_RATES    = 0x20;
    PPARAM_EXTRA1        = 0x21;
    PPARAM_EXTRA2        = 0x22;
    PPARAM_B_SOUND       = 0x23;

---------------------------------------------------------------------
-- SECTION: Sprite tags (see EXTRA_TAGFLAGS)

    TAG_REBORN           = 0x0001;
    TAG_ACTIVATED        = 0x0002;

---------------------------------------------------------------------
-- SECTION: Keycodes
    
    KEY_A                = 0x61;
    KEY_B                = 0x62;
    KEY_C                = 0x63;
    KEY_D                = 0x64;
    KEY_E                = 0x65;
    KEY_F                = 0x66;
    KEY_G                = 0x67;
    KEY_H                = 0x68;
    KEY_I                = 0x69;
    KEY_J                = 0x6a;
    KEY_K                = 0x6b;
    KEY_L                = 0x6c;
    KEY_M                = 0x6d;
    KEY_N                = 0x6e;
    KEY_O                = 0x6f;
    KEY_P                = 0x70;
    KEY_Q                = 0x71;
    KEY_R                = 0x72;
    KEY_S                = 0x73;
    KEY_T                = 0x74;
    KEY_U                = 0x75;
    KEY_V                = 0x76;
    KEY_W                = 0x77;
    KEY_X                = 0x78;
    KEY_Y                = 0x79;
    KEY_Z                = 0x7a;
    KEY_0                = 0x30;
    KEY_1                = 0x31;
    KEY_2                = 0x32;
    KEY_3                = 0x33;
    KEY_4                = 0x34;
    KEY_5                = 0x35;
    KEY_6                = 0x36;
    KEY_7                = 0x37;
    KEY_8                = 0x38;
    KEY_9                = 0x39;
    KEY_F1               = 0x131;
    KEY_F2               = 0x132;
    KEY_F3               = 0x133;
    KEY_F4               = 0x134;
    KEY_F5               = 0x135;
    KEY_F6               = 0x136;
    KEY_F7               = 0x137;
    KEY_F8               = 0x138;
    KEY_F9               = 0x139;
    KEY_ESCAPE           = 0x100;
    KEY_CTRL             = 0x101;
    KEY_SHIFT            = 0x102;
    KEY_ALT              = 0x103;
    KEY_MENU             = 0x109;
    KEY_LBRACKET         = 0x10a;
    KEY_RBRACKET         = 0x10b;
    KEY_SEMICOLON        = 0x10c;
    KEY_COMA             = 0x10d;
    KEY_PERIOD           = 0x10e;
    KEY_QUOTE            = 0x10f;
    KEY_SLASH            = 0x110;
    KEY_BACKSLASH        = 0x111;
    KEY_TILDE            = 0x112;
    KEY_EQUAL            = 0x113;
    KEY_MINUS            = 0x114;
    KEY_SPACE            = 0x115;
    KEY_RETURN           = 0x116;
    KEY_BACKSPACE        = 0x117;
    KEY_TAB              = 0x118;
    KEY_PAGEUP           = 0x119;
    KEY_PAGEDOWN         = 0x11a;
    KEY_END              = 0x11b;
    KEY_HOME             = 0x11c;
    KEY_INSERT           = 0x11d;
    KEY_DELETE           = 0x11e;
    KEY_PLUS             = 0x11f;
    KEY_MINUS            = 0x120;
    KEY_MULTIPLY         = 0x121;
    KEY_DIVIDE           = 0x122;
    KEY_LEFT             = 0x123;
    KEY_RIGHT            = 0x124;
    KEY_UP               = 0x125;
    KEY_DOWN             = 0x126;
    KEY_PAUSE            = 0x140;
