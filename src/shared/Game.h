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
#include "../shared/Const.h"

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
    CActor & getPlayer();
    CScene & scene();
    CLevel *layers();
    CMap & map();
    void removePointsOBL();
    void cacheImages();
    static int getVersion();
    int testKey(int keyCode);
    void setKey(int keyCode, char value);
    void setJoyButton(lgck::Button::JoyButton button, char value);
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
        STATE_VERSION       = 0,
        INVALID             = -1
    };

    enum : uint16_t {
        MAX_HP              = 255,
        PROTO_SPECIAL_MASK  = 0x8000,
        PROTO_POINTS        = 0xffff,
        PROTO_DEAD_PLAYER   = 0xfffe,
        PROTO_DEAD_WOOD     = 0xfffd,
        DEFAULT_TICK_RATE   = 90
    };

    enum JoyState{
        JOY_UP              = 0x0001,
        JOY_DOWN            = 0x0002,
        JOY_LEFT            = 0x0004,
        JOY_RIGHT           = 0x0008,
        JOY_JUMP            = 0x0010,
        JOY_FIRE            = 0x0020,
        JOY_ZKEY            = 0x0040,
        JOY_SPECIAL1        = 0x0080,
        JOY_SPECIAL2        = 0x0100
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

    typedef struct
    {
        lgck::Key::Code keyCode;
        lgck::Button::JoyButton button;
    } JoyStateEntry;

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
    CScene *_fw();
    CScene *_bk();
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
    uint64_t startTime();

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
    void autoCenter (CActor & player, uint32_t nAim);
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
    uint32_t getBkColor();
    void setBkColor(uint32_t bkColor);

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
    uint32_t getJoyState();
    int whoIs(int x, int y);
    const char *keys();
    JoyStateEntry & joyStateEntry(int i);
    const char * buttonText(int i);
    int findButtonText(const char *text);
    bool isJoyActionOn(int action);
    void clearActionKey(int action);

    bool exportJoyStateMap(IFile & file);
    bool importJoyStateMap(IFile & file);

    /////////////////////////////////////////////////////////////////
    // displayManager
    void setDisplayState(bool state, int alpha);
    void setDisplayState(bool state);
    bool getDisplayState();
    int getDisplayAlpha();
    void setDisplayAlpha(int alpha);
    void updateGeometry(int screenLen, int screenHei);
    void clearDisplay();   
    void restoreDisplays(CDisplayConfig *config);
    void saveDisplays(CDisplayConfig *config);

    /////////////////////////////////////////////////////////////////
    // luaBind
    bool readScript(const char *fname, std::string & out);
    void initLua();
    bool loadScript(const char *scriptName);
    void exec(const char* luaCode);
    void generateRuntimeLua(std::string & str);
    CInventory *getInventory(const char *name=nullptr);
    int & counter(const char *s);
    unsigned long long & var(const char *s);
    long long & svar(const char *s);
    unsigned int & var32(const char *s);
    void var_del(const char *s);
    std::string & strv(const char *s);
    void strv_del(const char *s);
    bool playSound(const char *name);
    bool playSound(int index);

    /////////////////////////////////////////////////////////////////
    // static
    static CGame & getGame();
    static CLuaVM & luaVM();
    static void error(const char *fnName, int argc);
    static void debug(const char *s);
    static unsigned int bgr2rgb(uint32_t bgr, int alpha=0xff);

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

    char *m_keys;
    char m_buttons[lgck::Button::Count];
    static const JoyStateEntry m_defaultJoyStateMap[lgck::Input::Count];
    JoyStateEntry m_joyStateMap[lgck::Input::Count];
    static const int m_arrPoints[];
    void copyDefaultJoyStateMap();

    int m_mx;
    int m_my;
    int m_screenLen;
    int m_screenHei;
    int BUFFERLEN;
    int BUFFERHEI;
    CMap *m_map;
    CFrameSet *m_points;
    CLevel *m_layers;
    CInventoryTable *m_inventoryTable;

    //////////////////////////////////////////////////////////////////
    // Game variables
    CScene *m_sFW;
    CScene *m_sBK;
    uint64_t m_startTime;
    static CLuaVM m_lua;
    static CGame *m_game;

    friend class CActor;
    friend class CAttacker;
    friend class IGraphics;
};

/////////////////////////////////////////////////////////////////////////////

#endif
