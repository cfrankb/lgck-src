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

#ifndef _GAME_H_20091207
#define _GAME_H_20091207

// Game.h header file
//

#include <unordered_map>
#include "../shared/GameFile.h"
#include "../shared/LuaVM.h"

class CGame;
class CSelection;
class CScene;
class CInventory;
class CInventoryTable;
class IMusic;
class CProto;
class CMap;
class IDisplayManager;
class CDisplayManager;
class CLayer;
class CActor;
class CMapEntry;
class IImageManager;
class IGraphics;
class ISound;
class CSnapshot;
class IFile;
class CFont;

/////////////////////////////////////////////////////////////////////////////
// CGame

class CGame : public CGameFile
{

public:

    CGame();
    ~CGame();

    int getTicks();
    CLevel * getLayers();
    CActor & getPlayer();
    CScene & scene();
    CMap & map();
    void removePointsOBL();
    void cacheImages();
    static int getVersion();
    int testKey(int keyCode);
    void setKey(int keyCode, char value);
    void stopMusic();
    void setLastKey(int keyCode);
    void callObjEvent(int objId, int eventId);
    void callLvEvent(int eventId);
    void callGameEvent(int eventId);
    void clearKeys();
    int call(const char *fnName);
    int unfoldEvents();
    bool resetDefaultDisplays();
    void triggerMouseEvent(int x, int y, int button);

    enum {
        EVENT_NO_EVENT          = 0,
        EVENT_QUIT              = 1,
        EVENT_LEVEL_COMPLETED   = 2,
        EVENT_TIMEOUT           = 3,
        EVENT_PLAYER_DIED       = 4,
        EVENT_NO_PLAYER         = 5,
        EVENT_ERROR             = -1
    };

    void flipPause();

    // constants

    enum {
        STATE_VERSION      = 0
    };

    enum {
        MAX_HP              = 255,
        PROTO_SPECIAL_MASK  = 0x8000,
        PROTO_POINTS        = 0xffff,
        PROTO_DEAD_PLAYER   = 0xfffe,
        PROTO_DEAD_WOOD     = 0xfffd,
        DEFAULT_TICK_RATE   = 90,
        INVALID             = -1
    };

    enum {
        JOY_UP          = 1,
        JOY_DOWN		= 2,
        JOY_LEFT		= 4,
        JOY_RIGHT		= 8,
        JOY_JUMP		= 16,
        JOY_FIRE		= 32,
        JOY_ZKEY		= 64
    };

    enum {
        DI_NONE             = 0,
        DI_ANIMATION        = 1,
        DI_REMOVAL          = 2
    };

    enum {
        BUTTON_LEFT         = 1,
        BUTTON_RIGHT        = 2,
        BUTTON_MIDDLE       = 3
    };

    enum {
        ES_INTRO,
        ES_PLAYLEVEL,
        ES_TIMEOUT
    };

    /////////////////////////////////////////////////////////////////
    // Settings

    bool initSettings();

    /////////////////////////////////////////////////////////////////
    // Level

    bool calculateWorldSize(CLevel *s, int &width, int &height);
    void hideSystemObject (CLayer & layer, bool bHide);
    void splitLevel (CLevel *sS, CScene *sBK, CScene * sFW);
    void moveTo (int x, int y);
    void moveBy (int x, int y);
    bool initLevel (int n);
    bool initLevelTest(int level, int skill, bool initSound);
    void postInitLevel ();
    void triggerPlayerHitState();
    void setClosure(int event);
    int getLevel();
    void setLevel(int level);
    void nextLevel();
    bool isEndLevelMeet();
    void setEndLevel(bool b);
    bool isLevelEnded();
    int & _mx();
    int & _my();
    CSnapshot & snapshot();

    /////////////////////////////////////////////////////////////////
    // wrap
    void setWrap(int wrapFlags);
    int getWrap();

    /////////////////////////////////////////////////////////////////
    // gravity
    void setGravity(bool b);
    bool gravity();

    /////////////////////////////////////////////////////////////////
    // ticks
    void resetTicks();
    int getTickCount();
    void nextTick();

    /////////////////////////////////////////////////////////////////
    // timeLeft
    void setTimeLeft(int timeLeft);
    int getTimeLeft();
    void decTimeLeft();
    void setNextSecond(int ts);
    int getNextSecond();

    /////////////////////////////////////////////////////////////////
    // trigger
    bool wasTriggerCalled();
    void setTriggerState(bool state);
    void callTrigger(const int key);

    /////////////////////////////////////////////////////////////////
    // Game play functions

    bool initFonts();
    bool initSounds();
    char *m_keys;
    void attach(IImageManager *im);
    void attach(IGraphics *gr);
    void attach(IMusic *mu);
    void attach(ISound *sn);

    // sprites
    void killPlayer(CActor & player);
    const char *getAimName(int aim);

    /////////////////////////////////////////////////////////////////
    // Player specific functions

    void managePlayerJump();
    bool managePlayer();
    void managePlayerMovements (CActor & player);
    void managePlyTimerOutCounter (CActor & player);
    bool managePlayerFiring (CActor & player);
    void autoCenter (CActor & player, UINT32 nAim);
    bool centerOnPlayer (CActor & player);
    bool playerZKey();
    int manageKeyEvents();
    void manageTasks();
    void updateHP();
    void addToScore(int nPoints);
    void addToHP(int hp);

    // health & lives
    int getLives();
    void setLives(int lives);
    void setHealth(int hp);

    // vitals
    void setVitals(int hp, int lives,int score);
    void getVitals(int & hp, int & lives, int & score);

    // background color
    UINT32 getBkColor();
    void setBkColor(UINT32 bkColor);

    // lookup
    void setLookUp(bool set);
    bool getLookUp();

    // levelGoal
    void setLevelGoal(int goal);
    int getLevelGoal();

    // pause
    bool getPause();
    void setPause(bool state);

    // skill level
    void setSkill(int skill);
    int getSkill();

    // score
    void setScore(int score);
    int getScore();

    /////////////////////////////////////////////////////////////////
    // input device

    bool keyPressed();
    void updateJoyState();
    UINT32 getJoyState();
    int whoIs(int x, int y);

    /////////////////////////////////////////////////////////////////
    // displayManager
    void setDisplayState(bool state, int alpha);
    void setDisplayState(bool state);
    bool getDisplayState();
    int getDisplayAlpha();
    void setDisplayAlpha(int alpha);
    void updateGeometry(int screenLen, int screenHei);
    void clearDisplay();   

    /////////////////////////////////////////////////////////////////
    // luaBind
    bool readScript(const char *fname, std::string & out);
    void initLua();
    bool loadScript(const char *scriptName);
    void exec(const char* luaCode);
    void generateRuntimeLua(std::string & str);
    CInventory *getInventory(const char *name=NULL);
    int & counter(const char *s);
    unsigned long long & var(const char *s);
    long long & svar(const char *s);
    unsigned int & var32(const char *s);
    void var_del(const char *s);
    std::string & strv(const char *s);
    void strv_del(const char *s);
    bool playSound(const char *name);
    bool playSound(int index);

    //////////////////////////////////////////////////////////////////
    // Game variables
    int m_screenLen;
    int m_screenHei;
    int BUFFERLEN;
    int BUFFERHEI;
    unsigned long long m_startTime;
    CScene *m_sFW;
    CScene *m_sBK;
    CMap *m_map;
    CFrameSet *m_points;
    CLevel *m_layers;
    int m_mx;
    int m_my;
    CInventoryTable *m_inventoryTable;

    /////////////////////////////////////////////////////////////////
    // static
    static CGame *m_game;
    static CLuaVM m_lua;
    static int m_arrPoints[];
    static CGame & getGame();
    static void error(const char *fnName, int argc);
    static void debug(const char *s);
    static unsigned int bgr2rgb(unsigned int bgr, int alpha=0xff);
    friend class CScene;
    friend class CActor;
    friend class CDisplayManager;

    /////////////////////////////////////////////////////////////////
    // classes
    static bool isPlatformClass(int classId);
    static bool isBackgroundClass(int classId);

    IDisplayManager *displays();
    IGraphics * graphics();
    IMusic *music();
    ISound *sound();

    /////////////////////////////////////////////////////////////////
    // engine
    void setEngineState(unsigned int state);
    int runEngine();
    void updateScreen();
    void saveGame(IFile &file);
    void loadGame(IFile & file);
    void remap();

protected:
    std::unordered_map<std::string, int> m_counters;
    std::unordered_map<std::string, unsigned long long> m_vars;
    std::unordered_map<std::string, std::string> m_strings;
    IGraphics *m_graphics;
    IMusic *m_music;
    ISound *m_sound;
    CSnapshot *m_snapshot;
    CTasks *m_tasks;
    CFont *m_font;
};

/////////////////////////////////////////////////////////////////////////////

#endif
