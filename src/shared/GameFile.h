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

#ifndef GAMEFILE_H
#define GAMEFILE_H

#include "FrameArray.h"
#include "ProtoArray.h"
#include "SndArray.h"
#include "Settings.h"

class CSelection;
class CStringTable;
class CPathBlock;
class CLevel;
class IImageManager;
class CTasks;
class CGameEvents;
class CSettings;

typedef struct
{
    std::string name;
    std::string value;
    std::string desc;
} CONST_DATA;

class CGameFile
{

public:

    CGameFile();
    ~CGameFile();

    void init();
    void forget();

    /////////////////////////////////////////////////////////////////
    // public enums

    enum {
        UP          	= 0,
        DOWN			= 1,
        LEFT			= 2,
        RIGHT			= 3,
        AIM_MASK        = 3,
        LEAP            = 0x10,
        LEAP_LEFT		= 0x12,
        LEAP_RIGHT		= 0x13,
        LEFT_UP			= 4,
        RIGHT_UP		= 5,
        LEFT_DOWN		= 6,
        RIGHT_DOWN		= 7,
        PAUSE			= -2,
        SEPARATOR		= -1,
        FALL            = 254,
        HERE			= 255
    } SPRITE_AIM;

    enum {
        TRIGGER_KEYS    = 0x1f,
        TRIGGER_GOAL    = 0x20,
        TRIGGER_FROZEN  = 0x40,
        TRIGGER_HIDDEN	= 0x80,
        TRIGGER_MASK    = 0xf0
    } TRIGGER;

    // settings
    enum {
        PARAM_TITLE             = 0,
        PARAM_ICON              = 1,
        PARAM_AUTHOR            = 2,    // tba
        PARAM_DESCRIPTION       = 3,    // tba
        PARAM_DEFAULT_SPEED     = 4,    // tba
        PARAM_AUTO_CENTER_GR    = 5,    // tba
        PARAM_SCREEN_W          = 6,
        PARAM_SCREEN_H          = 7,
        PARAM_FULLSCREEN        = 8,
        PARAM_SIGNS             = 9,
        PARAM_POINTS            = 10,
        PARAM_LOOKUP            = 11,
        PARAM_SYMBOLS           = 12,
        PARAM_TIMEOUT           = 13,
        PARAM_WALK_DELAY        = 14,   // obsolete
        PARAM_FALL_SPEED        = 15,   // obsolete
        PARAM_MAX_FALL          = 16,   // obsolete
        PARAM_LEVEL_HELLO       = 17,
        PARAM_STRETCH           = 18,
        PARAM_JUMP_UP           = 19,   // obsolete
        PARAM_JUMP_DOWN         = 20,   // obsolete
        PARAM_JUMP_LEFT         = 21,   // obsolete
        PARAM_JUMP_RIGHT        = 22,   // obsolete
        PARAM_JUMP_UP_LEFT      = 23,   // obsolete
        PARAM_JUMP_UP_RIGHT     = 24,   // obsolete
        PARAM_JUMP_DOWN_LEFT    = 25,   // obsolete
        PARAM_JUMP_DOWN_RIGHT   = 26,   // obsolete
        PARAM_AUTO_CENTER_SPEED = 27,
        PARAM_LIVES             = 28,
        PARAM_HP                = 29,
        PARAM_LIVES_MAX         = 30,
        PARAM_HP_MAX            = 31
    } PARAM_GAME;

    /////////////////////////////////////////////////////////////////
    // variables

    int                 m_nLevels;
    int                 m_nCurrLevel;
    bool                m_dirty;
    std::string         m_fileName;
    std::string         m_path;

    // graphics
    CFrameArray 		m_arrFrames;

    // objects
    CProtoArray     	m_arrProto;

    // scripts
    CLevel              **m_arrLevels;

    // sounds
    CSndArray           m_arrSounds;

    // classes
    enum {
        MAX_CLASSES = 256
    };

    std::string m_className [ MAX_CLASSES ];
    std::string m_classInfo [ MAX_CLASSES ];
    UINT8 m_classType [ MAX_CLASSES ];
    UINT8 m_classLock [ MAX_CLASSES ];

    static unsigned int getVersion();
    bool isUntitled ();
    bool isDirty() const;
    void setDirty(const bool dirty);
    int whoIs(CLevel & script, int x, int y);
    const char * getFileName();
    void setFileName(const char * fileName);
    int getSize();
    const char* getLastError() const;

    CLevel * operator [] (int n);
    CLevel * getLevel(int i);
    int getLevelByUUID(const char *uuid);
    void insertLevel(int n, CLevel *level);
    void addLevel(CLevel *level);
    void removeLevel (int n);
    void killProto(int nProto);
    void killFrameSet (int nFrameSet);
    CSelection * getClipboard();
    CStringTable * getStringTable();
    CPathBlock * getPaths();
    CGameEvents * getEvents();
    CSettings * getSettings();

    /////////////////////////////////////////////////////////////////
    // i/o

    bool read();
    bool write();

    /////////////////////////////////////////////////////////////////
    // settings

    void restoreDefaults();
    IImageManager * cache();
    CONST_DATA * getSpriteList();
    CONST_DATA * getClassList();
    CONST_DATA * getImageList();
    CONST_DATA * getSoundList();

protected:
    IImageManager *m_imageManager;
    CStringTable *m_strings;
    CPathBlock *m_paths;
    CGameEvents *m_events;
    CSettings *m_settings;
    typedef struct {
        std::string name;
        std::string src;
    } SOUND;
    typedef struct {
        UINT32 id;
        UINT32 game_uid;
        UINT32 filler[2];
    } VERSION;
    std::string m_lastError;
    CSelection *m_clipboard;
    static CSettings::SETTING m_gameDefaults[];
    static SOUND m_defaultSounds[];
    void initDefaultSounds();
    void parseClassList();
};

#endif
