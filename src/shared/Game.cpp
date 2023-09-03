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

// Game.cpp : implementation file
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <cassert>
#include "../shared/Game.h"
#include "../shared/vlamits3.h"
#include "../shared/Inventory.h"
#include "../shared/inventoryTable.h"
#include "../shared/Map.h"
#include "../shared/Scene.h"
#include "../shared/Snd.h"
#include "../shared/Frame.h"
#include "../shared/Level.h"
#include "../shared/Layer.h"
#include "../shared/interfaces/IImageManager.h"
#include "../shared/interfaces/IDisplayManager.h"
#include "../shared/interfaces/IGraphics.h"
#include "../shared/interfaces/ISound.h"
#include "../shared/interfaces/IMusic.h"
#include "../shared/Display.h"
#include "../shared/Tasks.h"
#include "../shared/GameEvents.h"
#include "../shared/Const.h"
#include "../shared/helper.h"
#include "../shared/Snapshot.h"
#include "../shared/Extra.h"
#include "../shared/IFile.h"
#include "../shared/Font.h"
#include "displayconfig.h"
#include "fontmanager.h"
#include "Countdown.h"
#include "microtime.h"

#ifdef USE_QFILE
#define FILEWRAP QFileWrap
#include "../shared/qtgui/qfilewrap.h"
#else
#define FILEWRAP CFileWrap
#include "../shared/FileWrap.h"
#endif

constexpr char SIGNATURE_JOYSTATEMAP[] = "JOYS";
constexpr uint32_t REVISION_JOYSTATEMAP = 0;

#define JM_VLA3 0
#define JM_GIANA 1
#define JM_MIXED 2

// http://git.postgresql.org/gitweb/?p=postgresql.git;a=blob;f=src/port/gettimeofday.c;h=75a91993b74414c0a1c13a2a09ce739cb8aa8a08;hb=HEAD

/////////////////////////////////////////////////////////////////////////////
// CGame

const int CGame::m_arrPoints[] =
    {
        0,
        10,
        25,
        50,
        100,
        200,
        300,
        400,
        500,
        1000,
        2000,
        5000,
        10000};

const CGame::JoyStateEntry CGame::m_defaultJoyStateMap[lgck::Input::Count] = {
    {lgck::Key::Up, lgck::Button::Up},
    {lgck::Key::Down, lgck::Button::Down},
    {lgck::Key::Left, lgck::Button::Left},
    {lgck::Key::Right, lgck::Button::Right},
    {lgck::Key::Space, lgck::Button::A},
    {lgck::Key::LShift, lgck::Button::B},
    {lgck::Key::Z, lgck::Button::Invalid}, // action (z-key)
    {lgck::Key::Invalid, lgck::Button::Invalid},
    {lgck::Key::Invalid, lgck::Button::Invalid}};

constexpr char gamePadButtons[][8]{
    "A",
    "B",
    "X",
    "Y",
    "L1",
    "L3",
    "R1",
    "R3",
    "Select",
    "Start",
    "Up",
    "Down",
    "Left",
    "Right",
    "Center",
    "Guide"};

CGame::CGame() : CGameFile()
{
    // Game General Init
    m_map = new CMap();
    setLevel(0);
    m_keys = new char[lgck::Key::Count];
    clearKeys();
    m_music = nullptr;
    m_inventoryTable = new CInventoryTable();
    m_inventoryTable->reset(true);
    m_points = nullptr;
    m_game = this;
    var("skill") = 0;
    var("level") = 0;
    m_mx = 0;
    m_my = 0;
    counter("coins") = 0;
    counter("lives") = 0;
    counter("endLevel") = 0;
    m_layers = new CLevel;
    var("pointsOBL_texture") = 0;
    m_sBK = new CScene();
    m_sFW = new CScene();
    m_sBK->setOwner(this, true);
    m_sFW->setOwner(this, false);
    CActor::setGame(this);
    m_graphics = nullptr;
    m_sound = nullptr;
    svar("WarpTo") = INVALID;
    m_snapshot = new CSnapshot;
    m_tasks = new CTasks;
    copyDefaultJoyStateMap();
    m_countdowns = new CCountdown;
}

CGame::~CGame()
{
    m_arrSounds.forget();
    forget();

    if (m_points)
    {
        delete m_points;
        m_points = nullptr;
    }

    if (m_layers)
    {
        delete m_layers;
        m_layers = nullptr;
    }

    if (m_imageManager)
    {
        delete m_imageManager;
    }

    delete[] m_keys;
    delete m_inventoryTable;
    delete m_map;
    delete m_sBK;
    delete m_sFW;
    delete m_snapshot;
    delete m_tasks;
    delete m_countdowns;
    m_countdowns = nullptr;
}

/////////////////////////////////////////////////////////////////////
// attributes

int CGame::getTicks()
{
    return getTickCount();
}

CLevel *CGame::layers()
{
    return m_layers;
}

/////////////////////////////////////////////////////////////////////////////
// cpp interface

void CGame::updateJoyState()
{
    unsigned int state = 0;
    int flagValue = 1;
    for (unsigned int i = 0; i < sizeof(m_joyStateMap) / sizeof(JoyStateEntry); ++i)
    {
        const JoyStateEntry &entry = m_joyStateMap[i];
        state |= ((entry.keyCode >= 0 ? m_keys[entry.keyCode] : 0) |
                  (entry.button >= 0 ? m_buttons[entry.button] : 0)) *
                 flagValue;
        flagValue *= 2;
    }
    var("joyState") = state;
}

bool CGame::isJoyActionOn(int action)
{
    const JoyStateEntry &entry = m_joyStateMap[action];
    return ((entry.keyCode >= 0 ? m_keys[entry.keyCode] : 0) |
            (entry.button >= 0 ? m_buttons[entry.button] : 0)) != 0;
}

void CGame::clearActionKey(int action)
{
    const JoyStateEntry &entry = m_joyStateMap[action];
    entry.keyCode >= 0 ? m_keys[entry.keyCode] = 0 : 0;
    entry.button >= 0 ? m_buttons[entry.button] = 0 : 0;
}

/////////////////////////////////////////////////////////////////////
// bkColor

uint32_t CGame::getBkColor()
{
    return var("bkColor");
}

void CGame::setBkColor(uint32_t bkColor)
{
    var("bkColor") = bkColor;
}

/////////////////////////////////////////////////////////////////////
// wrap

void CGame::setWrap(int wrapFlags)
{
    var("wrapFlags") = wrapFlags;
}

int CGame::getWrap()
{
    return var("wrapFlags");
}

/////////////////////////////////////////////////////////////////////
// endLevel

void CGame::setEndLevel(bool b)
{
    counter("endLevel") = (int)b;
}

bool CGame::isLevelEnded()
{
    return counter("endLevel");
}

int &CGame::_mx()
{
    return m_mx;
}

int &CGame::_my()
{
    return m_my;
}

// pause

bool CGame::getPause()
{
    return var("pause");
}

void CGame::setPause(bool state)
{

    var("pause") = (int)state;
    int i = displays()->findDisplay("pause");
    if (i != INVALID)
    {
        CDisplay &display = displays()->getAt(i);
        display.setVisible(var("pause") != 0);
    }
}

void CGame::flipPause()
{

    var("pause") ^= 1;
    int i = displays()->findDisplay("pause");
    if (i != INVALID)
    {
        CDisplay &display = displays()->getAt(i);
        display.setVisible(var("pause") != 0);
    }
}

// level

int CGame::getLevel()
{
    return var("level");
}

void CGame::setLevel(int level)
{
    var("level") = level;
}

void CGame::nextLevel()
{
    ++var("level");
}

// score

void CGame::setScore(int score)
{
    counter("score") = score;
}

void CGame::addToScore(int pts)
{
    counter("score") += pts;
}

int CGame::getScore()
{
    return counter("score");
}

// trigger

bool CGame::wasTriggerCalled()
{
    return var("__levelTriggerCalled");
}

void CGame::setTriggerState(bool state)
{
    var("__levelTriggerCalled") = state;
}

// timeLeft

void CGame::setTimeLeft(int timeLeft)
{
    counter("timeLeft") = timeLeft;
}

int CGame::getTimeLeft()
{
    return counter("timeLeft");
}

void CGame::decTimeLeft()
{
    if (counter("timeLeft") > 0)
    {
        --counter("timeLeft");
    }
}

// nextSecond

void CGame::setNextSecond(int ts)
{
    counter("nextSecond") = ts;
}

int CGame::getNextSecond()
{
    return counter("nextSecond");
}

// lives

int CGame::getLives()
{
    return counter("lives");
}

void CGame::setLives(int lives)
{
    counter("lives") = lives;
}

// hp

void CGame::setDisplayState(bool state, int alpha)
{
    counter("hp_state") = state;
    counter("hp_alpha") = alpha;
}

void CGame::setDisplayState(bool state)
{
    counter("hp_state") = state;
}

bool CGame::getDisplayState()
{
    return counter("hp_state");
}

int CGame::getDisplayAlpha()
{
    return counter("hp_alpha");
}

void CGame::setDisplayAlpha(int alpha)
{
    counter("hp_alpha") = alpha;
}

// ticks

void CGame::resetTicks()
{
    var("ticks") = 0;
}

int CGame::getTickCount()
{
    return var("ticks");
}

void CGame::nextTick()
{
    ++var("ticks");
}

// joyState

uint32_t CGame::getJoyState()
{
    return var("joyState");
}

// display

void CGame::updateGeometry(int screenLen, int screenHei)
{
    m_screenLen = screenLen;
    m_screenHei = screenHei;
}

void CGame::clearDisplay()
{
    displays()->removeAll();
}

void CGame::setSkill(int skill)
{
    var("skill") = skill;
}

int CGame::getSkill()
{
    return var("skill");
}

void CGame::setHealth(int hp)
{
    if (svar("playerEntry"))
    {
        CActor &player = (*m_sFW)[svar("playerEntry")];
        // player.m_hp = hp;
        player.set(lgck::EXTRA_HP, hp);
    }
}

void CGame::setGravity(bool b)
{
    var("gravity") = b;
}

bool CGame::gravity()
{
    return var("gravity");
}

/////////////////////////////////////////////////////////////////////
// operations

void CGame::cacheImages()
{
    if (m_imageManager)
    {
        m_imageManager->forget();
        for (int i = 0; i < m_arrFrames.getSize(); ++i)
        {
            m_imageManager->add(m_arrFrames[i]);
        }
    }
    else
    {
        CLuaVM::debugv("IImageManager not implemented");
    }
}

bool CGame::keyPressed()
{
    for (unsigned int i = 0; i < sizeof(m_keys); ++i)
    {
        if (m_keys[i])
        {
            return true;
        }
    }
    return false;
}

void CGame::clearKeys()
{
    for (unsigned int i = 0; i < lgck::Key::Count; ++i)
    {
        m_keys[i] = 0;
    }
    for (unsigned int i = 0; i < lgck::Button::Count; ++i)
    {
        m_buttons[i] = 0;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Sounds

bool CGame::initSounds()
{
    for (int i = 1; i < m_arrSounds.getSize(); ++i)
    {
        CSnd *snd = m_arrSounds[i];
        if (!m_sound->has_sound(snd->getUID()))
        {
            m_sound->add((unsigned char *)snd->getData(), (unsigned)snd->getSize(), snd->getUID());
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// CLevel

void CGame::splitLevel(CLevel *level, CScene *sBK, CScene *sFW)
{
    CProtoArray &arrProto = m_arrProto;
    CLayer *main = level->getMainLayer();
    if (!main)
    {
        CLuaVM::debugv("main layer is null\n");
        return;
    }

    // skill mask is 1, 2, 4 or 8
    int mask = 1 << getSkill();

    // insert dummy entry at pos 0
    CActor tmp;
    sFW->add(tmp);
    for (int n = 0; n < main->getSize(); ++n)
    {
        const CLevelEntry &entry = (*main)[n];
        // ignore sprites outside canvas
        if (entry.m_nX >= BUFFERLEN ||
            entry.m_nY >= BUFFERHEI)
        {
            continue;
        }
        if (entry.m_nActionMask & mask)
        {
            if (CGame::isBackgroundClass(arrProto[entry.m_nProto].m_nClass))
            {
                sBK->add(entry);
            }
            else
            {
                sFW->add(entry);
            }
        }
    }
}

bool CGame::calculateWorldSize(CLevel *s, int &width, int &height)
{
    int largerX = INVALID;
    int largerY = INVALID;
    CLayer *main = s->getMainLayer();
    if (!main)
    {
        CLuaVM::debugv("main is null\n");
        return false;
    }
    for (int i = 0; i < main->getSize(); ++i)
    {
        CLevelEntry &entry = (*main)[i];
        CFrameSet &frameSet = *m_arrFrames[entry.m_nFrameSet];
        CFrame &frame = *frameSet[entry.m_nFrameNo];

        if (entry.m_nX + frame.m_nLen > largerX)
        {
            largerX = entry.m_nX + frame.m_nLen;
        }

        if (entry.m_nY + frame.m_nHei > largerY)
        {
            largerY = entry.m_nY + frame.m_nHei;
        }
    }
    width = largerX;
    height = largerY;
    return true;
}

void CGame::restoreDisplays(CDisplayConfig *config)
{
    IDisplayManager *dm = displays();
    CDisplayConfig &conf = *config;
    for (int i = 0; i < m_displayConfig->getSize(); ++i)
    {
        // copy displays from displayConf to DisplayManager
        CDisplay *display = conf[i];
        dm->add(*display);
    }
}

void CGame::saveDisplays(CDisplayConfig *config)
{
    IDisplayManager *dm = displays();
    CDisplayConfig &conf = *config;
    config->forget();
    for (int i = 0; i < dm->getSize(); ++i)
    {
        // copy displays from displayConf to DisplayManager
        conf.add(dm->getAt(i));
    }
}

bool CGame::resetDefaultDisplays()
{
    IDisplayManager *dm = displays();
    if (!dm)
    {
        CLuaVM::debugv("IDisplayManager not attached\n");
        return false;
    }
    clearDisplay();
    restoreDisplays(m_displayConfig);
    return true;
}

unsigned int CGame::bgr2rgb(uint32_t bgr, int alpha)
{
    int blue = bgr & 0xff;
    int green = (bgr >> 8) & 0xff;
    int red = (bgr >> 16) & 0xff;
    return red + (green << 8) + (blue << 16) + (alpha << 24);
}

bool CGame::initLevel(int n)
{
    setLevel(n);
    CLevel *s = m_arrLevels[n];

    // remove remnant task
    m_tasks->forget();

    // reload snapshot
    bool snapshotReloading = false;
    if (m_snapshot->has_snapshot())
    {
        m_snapshot->reload(*this);
        snapshotReloading = true;
        // if this is a restart call the
        // restart event.
        // callLvEvent(CLevel::EL_RESTART);
    }

    BUFFERLEN = s->getSettingInt("width");
    BUFFERHEI = s->getSettingInt("height");
    if (!BUFFERLEN || !BUFFERHEI)
    {
        // calculate the size of the terrain
        if (!calculateWorldSize(s, BUFFERLEN, BUFFERHEI))
        {
            m_lastError = "Missing main layer (null)";
            return false;
        }
    }

    CLuaVM::debugv("opening music");
    // start music
    const char *music = s->getSetting("music");
    if (music[0])
    {
        std::string filePath;
        filePath = m_path.c_str() + std::string(music);
        if (m_music && m_music->open(filePath.c_str()))
        {
            m_music->play();
        }
        else
        {
            CLuaVM::debugv("failed to music");
        }
    }

    // init LuaVM
    initLua();

    if (!snapshotReloading)
    {
        // Split the background from the sprites
        m_sBK->forget();
        m_sFW->forget();
        splitLevel(s, m_sBK, m_sFW);
        // copy the other layers here
        m_layers->forget();
        for (int i = 0; i < s->getSize(); ++i)
        {
            CLayer *layer = new CLayer();
            *layer = (*s)[i];
            m_layers->addLayer(layer);
        }
    }
    svar("WarpTo") = INVALID;

    // Save the background rgb value for future use
    var("bkColor") = bgr2rgb(strtol(s->getSetting("bkcolor"), nullptr, 16));

    // Save the border rgb value for future use
    var("borderColor") = bgr2rgb(strtol(s->getSetting("borderColor"), nullptr, 16));

    // Save colorMod
    var("colorMod") = strtol(s->getSetting("colorMod"), nullptr, 16);

    // allocate the collision map
    if (!snapshotReloading)
    {
        Size sx = CMap::size(BUFFERLEN, BUFFERHEI);
        var("BUFFERLEN") = BUFFERLEN;
        var("BUFFERHEI") = BUFFERHEI;
        m_map->resize(sx.len, sx.hei);

        // Map both background and forewardground objects
        m_sBK->map();
        m_sFW->map();
    }

    // Find the player object
    if ((svar("playerEntry") = m_sFW->findPlayerEntry()) == INVALID)
    {
        m_lastError = "Missing player object!";
        return false;
    }

    // Hide invisible systemObject objects
    m_sBK->hideSystemObject(true);
    m_sFW->hideSystemObject(true);

    setWrap(strtol(s->getSetting("wrap"), nullptr, 10));
    if (!snapshotReloading)
    {
        if (!resetDefaultDisplays())
        {
            m_lastError = "Display Manager not attached!";
            return false;
        }
    }
    setPause(false);
    setDisplayState(true, 0);
    setTriggerState(false);
    resetTicks();

    // Init PlayerObject
    CLuaVM::debugv("init player\n");
    if (svar("playerEntry") != INVALID)
    {
        CActor &player = (*m_sFW)[svar("playerEntry")];
        player.callEvent(CObject::EO_ACTIVATE);
        centerOnPlayer(player);
        player.setState(CHitData::STATE_HIT | CHitData::STATE_BEGINNING, true);
        if (!snapshotReloading)
        {
            player.set(lgck::EXTRA_HP, 50);
        }
        player.set(lgck::EXTRA_TIMEOUT, var("CONST_TIMEOUT"));
    }

    int tick_rate = s->getSettingInt(CLevel::SPARAM_TICK_RATE);
    if (!tick_rate)
    {
        tick_rate = DEFAULT_TICK_RATE;
        CLuaVM::debugv("default tick rate\n");
    }

    var("TICK_SCALE") = 1000 / tick_rate;
    setTimeLeft(s->getSettingInt("time"));

    // level specific settings
    CLuaVM::debugv("init level settings\n");
    m_startTime = microtime();
    var("startTime") = m_startTime;
    counter("nextTick") = var("TICK_SCALE");
    setNextSecond(1000);

    setGravity(s->getSettingInt(CLevel::SPARAM_NO_GRAVITY) == 0);
    counter("goalLeft") = m_sFW->countGoals();
    setEndLevel(false);
    setLevelGoal(s->getSettingInt(CLevel::SPARAM_GOAL));
    setLookUp((*m_settings)[PARAM_LOOKUP].valueInt > 0 && !(s->getSettingInt(CLevel::SPARAM_WRAP) & CLevel::NO_LOOK_UP));
    counter("closureEvent") = EVENT_NO_EVENT;
    counter("closureTime") = 0;
    counter("closureDelay") = s->getSettingInt("closure");
    // reset hard Inventory table
    m_inventoryTable->reset(true);
    m_inventoryTable->addInventory("@player");

    return true;
}

void CGame::moveBy(int x, int y)
{
    m_mx += x;
    m_my += y;
}

void CGame::moveTo(int x, int y)
{
    m_mx = x;
    m_my = y;
}

bool CGame::managePlayer()
{
    updateJoyState();
    if (svar("playerEntry") == INVALID)
    {
        return false;
    }

    CActor &player = getPlayer();
    player.set(lgck::EXTRA_FLAGS, player.checkHit());
    player.callEvent(CObject::EO_HANDLER);

    /////////////////////////////////////////////////////////////////
    // if the death flag is on, set the death state
    if (player.get(lgck::EXTRA_FLAGS) & CHitData::FLAG_DEATH)
    {
        player.setState(CHitData::STATE_DEAD, true);
    }

    /////////////////////////////////////////////////////////////////
    // Check if player is falling
    int bFall = player.canFall() && player.get(lgck::EXTRA_PATHDIR) == INVALID;
    /*
    if (bFall) {
        qDebug("grace:%d",player.m_fallGrace);
        if (player.m_fallGrace > 5) {
            qDebug("grace:%d FALL",player.m_fallGrace);
            player.setState(CHitData::STATE_FALL, bFall);
        } else {
            ++player.m_fallGrace;
            bFall = false;
        }
    } else {
        if (!player.canMove(DOWN) && !player.canFall()) {
            player.m_fallGrace = 0;
        }
    }*/

    if (!player.getState(CHitData::STATE_FALL) && bFall)
    {
        if (!player.getState(CHitData::STATE_DEAD))
        {
            player.callEvent(CObject::EO_FALL);
        }
        player.set(lgck::EXTRA_FALLHEIGHT, 0);
    }
    else
    {
        if (player.getState(CHitData::STATE_FALL) && !bFall)
        {
            if (!player.getState(CHitData::STATE_DEAD))
            {
                player.callEvent(CObject::EO_LAND);
                player.crash();
                CProto &proto = m_arrProto[player.m_nProto];
                if (proto.m_nMaxFall &&
                    player.get(lgck::EXTRA_FALLHEIGHT) > proto.m_nMaxFall)
                {
                    killPlayer(player);
                }
            }
            player.get(lgck::EXTRA_FALLHEIGHT) = 0;
        }
    }
    player.setState(CHitData::STATE_FALL, bFall);

    /////////////////////////////////////////////////////////////////
    // handle death sequence
    if (player.getState(CHitData::STATE_DEAD) && !player.get(lgck::EXTRA_DEATHINDICATOR))
    {
        player.set(lgck::EXTRA_DEATHINDICATOR, DI_ANIMATION);
        player.unMap();
        player.callEvent(CObject::EO_DEATH);
        if (!player.tryAnimation(CObject::AS_DEAD + player.m_nAim))
        {
            CLuaVM::debugv("player death animation failed (aim=0x%x)", player.m_nAim);
            player.set(lgck::EXTRA_DEATHINDICATOR, DI_REMOVAL);
        }
    }

    /////////////////////////////////////////////////////////////////
    // Control Player movements (while alive)
    if (!(player.m_nStateFlag & CHitData::STATE_DEAD))
    {
        // ZKey is not repeating
        if (isJoyActionOn(lgck::Input::Action))
        {
            clearActionKey(lgck::Input::Action);
            playerZKey();
        }
        if (!bFall && player.m_nProto > 0)
        {
            CProto &proto = m_arrProto[player.m_nProto];
            if (((!proto.m_nMoveSpeed) ||
                 (var("ticks") % proto.m_nMoveSpeed == 0)) &&
                !(player.get(lgck::EXTRA_FLAGS) & CHitData::FLAG_TELEPORT))
            {
                managePlayerMovements(player);
            }
        }
    }

    /////////////////////////////////////////////////////////////////
    // Manage Player Jumping
    if (player.get(lgck::EXTRA_PATHDIR) != INVALID && gravity())
    {
        player.managePath();
    }

    /////////////////////////////////////////////////////////////////
    // Manage TimeOut - how long after hit
    managePlyTimerOutCounter(player);

    /////////////////////////////////////////////////////////////////
    // Make the player fall
    CProto &proto = m_arrProto[player.m_nProto];
    if ((!proto.m_nFallSpeed || (var("ticks") % proto.m_nFallSpeed == 0)) && (bFall))
    {
        if ((player.m_nY < BUFFERHEI - 32) && player.canMove(FALL, false))
        {
            player.unMap();
            player.move(DOWN);
            player.map();
            player.autoCenter(DOWN);
            ++player.get(lgck::EXTRA_FALLHEIGHT);
        }
    }

    /////////////////////////////////////////////////////////////////
    // Manage Player firing
    if (!(player.m_nStateFlag & CHitData::STATE_DEAD))
    {
        managePlayerFiring(player);
    }

    return true;
}

bool CGame::playerZKey()
{
    if (svar("playerEntry") == INVALID)
        return false;
    CActor &player = getPlayer();
    if (player.m_nY < 0)
        return false;
    CScene &scene = (*m_sFW);
    CFrame *frame = m_arrFrames.getFrame(player);
    Size sx = CMap::size(frame);
    Pos p = m_map->toMap(player.m_nX, player.m_nY);
    for (int y = 0; y < sx.hei; ++y)
    {
        for (int x = 0; x < sx.len; ++x)
        {
            const CMapEntry &map = m_map->at(p.x + x, p.y + y);
            for (int i = 0; i < map.m_fwCount; ++i)
            {
                if (map.m_nFwClass[i] == CLASS_Z_KEY_SWITCH)
                {
                    CActor &object = scene[map.m_nFwEntry[i]];
                    object.callEvent(CObject::EO_ZKEY);
                    object.callTrigger();
                    player.callEvent(CObject::EO_ZKEY);
                    return true;
                }
            }
        }
    }
    return false;
}

const char *CGame::getAimName(int aim)
{
    const char *names[] = {
        "UP", "DOWN", "LEFT", "RIGHT"};

    const char *leap[] = {
        "LEAP_LEFT", "LEAP_RIGHT"};

    if (aim & LEAP)
    {
        return leap[aim & 1];
    }
    else
    {
        return names[aim];
    }
}

CActor &CGame::getPlayer()
{
    return (*m_sFW)[svar("playerEntry")];
}

bool CGame::centerOnPlayer(CActor &player)
{
    graphics()->getScreenSize(m_screenLen, m_screenHei);
    moveTo(0, 0);
    if (m_screenLen < BUFFERLEN && player.m_nX > m_screenLen / 2)
    {
        moveBy(std::min(BUFFERLEN - m_screenLen, player.m_nX - m_screenLen / 2), 0);
    }
    if (m_screenHei < BUFFERHEI && player.m_nY > m_screenHei / 2)
    {
        moveBy(0, std::min(player.m_nY - m_screenHei / 2, BUFFERHEI - m_screenHei));
    }
    return true;
}

void CGame::managePlyTimerOutCounter(CActor &player)
{
    uint32_t joyState = getJoyState();
    // Check hit - Dec TimeOutDelay
    if (!player.getState(CHitData::STATE_BEGINNING))
    {
        if (player.get(lgck::EXTRA_TIMEOUT) == 0)
        {
            player.setState(CHitData::STATE_HIT, false);
        }
        else
        {
            player.get(lgck::EXTRA_TIMEOUT)--;
        }
    }

    if (joyState && player.getState(CHitData::STATE_BEGINNING))
    {
        player.setState(CHitData::STATE_HIT | CHitData::STATE_BEGINNING, false);
    }
}

void CGame::managePlayerJump()
{
    uint32_t joyState = getJoyState();
    CActor &player = getPlayer();
    const CProto &proto = player.proto();
    if (gravity())
    {
        switch (proto.m_nJumpMode)
        {
        case JM_VLA3:
            if (joyState & JOY_JUMP)
            {

                switch (joyState & (-1 ^ (JOY_JUMP | JOY_FIRE)))
                {

                case JOY_UP:
                    player.tryPath(CObject::PS_JUMP_UP, UP);
                    break;

                case JOY_DOWN:
                    player.tryPath(CObject::PS_JUMP_DOWN, DOWN);
                    break;

                case JOY_LEFT | JOY_DOWN:
                    player.tryPath(CObject::PS_JUMP_DN_LEFT, LEFT);
                    break;

                case JOY_LEFT | JOY_UP:
                    player.tryPath(CObject::PS_JUMP_UP_LEFT, LEFT);
                    break;

                case JOY_LEFT:
                    player.tryPath(CObject::PS_JUMP_LEFT, LEFT);
                    break;

                case JOY_RIGHT | JOY_DOWN:
                    player.tryPath(CObject::PS_JUMP_DN_RIGHT, RIGHT);
                    break;

                case JOY_RIGHT | JOY_UP:
                    player.tryPath(CObject::PS_JUMP_UP_RIGHT, RIGHT);
                    break;

                case JOY_RIGHT:
                    player.tryPath(CObject::PS_JUMP_RIGHT, RIGHT);
                    break;
                }
            }
            break;

        case JM_GIANA:
            if (joyState & JOY_UP)
            {
                player.tryPath(CObject::PS_JUMP_UP, INVALID);
            }
            break;

        case JM_MIXED:
            if (joyState & JOY_JUMP)
            {
                player.tryPath(CObject::PS_JUMP_UP, INVALID);
            }
            break;
        }
    }
}

void CGame::managePlayerMovements(CActor &player)
{
    uint32_t joyState = getJoyState();
    bool lookup = false;
    // vla3 disallows control of movement during paths and fall
    CProto &proto = m_arrProto[player.m_nProto];
    if (proto.m_nJumpMode == JM_VLA3 &&
        (player.get(lgck::EXTRA_PATHDIR) != INVALID || player.getState(CHitData::STATE_FALL)))
    {
        return;
    }
    if (joyState)
    { // && player.m_pathDir == INVALID) {
        // manage player jump
        int nNewAim = INVALID;
        if (player.get(lgck::EXTRA_PATHDIR) == INVALID)
        {
            if (joyState & JOY_DOWN)
            {
                nNewAim = DOWN;
            }
            if (joyState & JOY_UP)
            {
                nNewAim = UP;
            }
            if (joyState & JOY_LEFT)
            {
                nNewAim = LEFT;
            }
            if (joyState & JOY_RIGHT)
            {
                nNewAim = RIGHT;
            }
            managePlayerJump();
        }
        // activate other animation sequences
        if ((nNewAim != INVALID) && (player.m_nAim != nNewAim || player.get(lgck::EXTRA_ANIMSEQ) == CObject::AS_DEFAULT))
        {
            player.m_nAim = nNewAim;
            player.tryAnimation(CObject::AS_MOVE + nNewAim);
        }
        else
        {
            if (player.m_nAim != HERE && player.get(lgck::EXTRA_ANIMSEQ) == INVALID && player.get(lgck::EXTRA_PATHDIR) == INVALID)
            {
                int animSeq = CObject::AS_MOVE + player.m_nAim;
                if (player.get(lgck::EXTRA_ANIMSEQ) != animSeq)
                {
                    player.tryAnimation(animSeq);
                }
            }
        }
        // Resume normal moving thing
        int &my = m_my;
        CFrame &frame = *m_arrFrames.getFrame(player);
        if (joyState & JOY_UP && player.get(lgck::EXTRA_PATHDIR) == INVALID)
        {
            CActor t_player;
            t_player.copyFrom(player);
            t_player.move(UP);
            if (player.canMove(UP, false) &&
                !t_player.canFall())
            {
                player.autoCenter(UP);
                player.unMap();
                player.move(UP);
                player.map();
                player.callEvent(CObject::EO_MOVE);
            }
            else
            {
                if (getLookUp() && gravity() && !getWrap() && !(joyState & (JOY_JUMP | JOY_FIRE)))
                {
                    lookup = true;
                    if ((my - 8 > 0) && (player.m_nY + frame.m_nHei <= my + m_screenHei))
                    {
                        my -= 8;
                    }
                }
            }
        }

        player.setState(CHitData::STATE_LOOKUP, lookup);

        if (joyState & JOY_DOWN && player.get(lgck::EXTRA_PATHDIR) == INVALID)
        {
            if (player.canMove(DOWN, false))
            {
                player.unMap();
                player.move(DOWN);
                player.map();
                player.autoCenter(DOWN);
                player.callEvent(CObject::EO_MOVE);
            }
            else
            {
                if (getLookUp() && !(joyState & (JOY_JUMP | JOY_FIRE)))
                {
                    lookup = true;
                    if ((my + m_screenHei + 8 < BUFFERHEI) && (player.m_nY >= my))
                    {
                        my += 8;
                    }
                }
            }
        }

        if ((joyState & JOY_LEFT))
        {
            if (player.canMove(LEFT, false))
            {
                player.unMap();
                player.move(LEFT);
                player.map();
                player.autoCenter(LEFT);
                player.callEvent(CObject::EO_MOVE);
            }
            else
            {
                CActor t_player;
                t_player.copyFrom(player);
                t_player.move(UP);
                if (player.m_nY != 0 &&
                    player.canMove(UP, false) &&
                    t_player.canMove(LEFT, false))
                {
                    player.unMap();
                    player.move(UP);
                    player.move(LEFT);
                    player.map();
                    player.callEvent(CObject::EO_LEAP);
                    player.autoCenter(LEFT);
                    player.autoCenter(UP);
                }
            }
        }

        if (joyState & JOY_RIGHT)
        {
            if (player.canMove(RIGHT, false))
            {
                player.unMap();
                player.move(RIGHT);
                player.map();
                player.autoCenter(RIGHT);
                player.callEvent(CObject::EO_MOVE);
            }
            else
            {
                CActor t_player;
                t_player.copyFrom(player);
                t_player.move(UP);
                player.autoCenter(RIGHT);
                if (player.m_nY != 0 &&
                    player.canMove(UP, false) && t_player.canMove(RIGHT, false))
                {
                    player.unMap();
                    player.move(UP);
                    player.move(RIGHT);
                    player.map();
                    player.callEvent(CObject::EO_LEAP);
                    player.autoCenter(RIGHT);
                    player.autoCenter(UP);
                }
            }
        }
    }
}

bool CGame::managePlayerFiring(CActor &player)
{
    // Manage Player Firing
    uint32_t joyState = getJoyState();
    if (joyState & JOY_FIRE)
    {
        player.setState(CHitData::STATE_FIRING, true);
    }
    else
    {
        player.setState(CHitData::STATE_FIRING, false);
    }
    // if a bulletProto was specified in properties
    // and bullet is enabled
    if (player.proto().m_nProtoBuddy &&
        player.proto().m_bulletOptions & CProto::BULLET_ENABLED)
    {
        callStaticHandler("firePlayerBullet", svar("playerEntry"));
    }
    player.callEvent(CObject::EO_FIRE);
    return true;
}

void CGame::callTrigger(const int nKey)
{
    if (nKey)
    {
        CScene &scene = (*m_sFW);
        for (int i = 0; i < scene.getSize(); ++i)
        {
            CActor &entry = scene[i];
            CProto &proto = m_arrProto[entry.m_nProto];
            if (!proto.getOption(CProto::OPTION_NO_TRIGGER_FLIP) &&
                (entry.m_nTriggerKey & TRIGGER_KEYS) == nKey)
            {
                entry.unMap();
                entry.m_nTriggerKey ^= entry.m_nActionMask & TRIGGER_MASK;
                entry.callEvent(CObject::EO_TRIGGER);
                if (entry.m_nActionMask & TRIGGER_DEATH_FLIP)
                {
                    entry.m_nActionMask ^= TRIGGER_DEATH_FLIP;
                    entry.callEvent(CObject::EO_DEATH);
                    entry.kill();
                }
                entry.map();
            }
        }
    }
}

void CGame::triggerPlayerHitState()
{
    CActor &player = getPlayer();
    player.callEvent(CObject::EO_HURT);
    player.setState(CHitData::STATE_HIT, true);
    if (!player.tryAnimation(CObject::AS_HURT + player.m_nAim))
    {
        // the hurt animation didn't work ! :(
    }
    player.set(lgck::EXTRA_TIMEOUT, var("CONST_TIMEOUT"));
}

int CGame::manageKeyEvents()
{
    if (m_keys[lgck::Key::Escape])
    {
        return EVENT_QUIT;
    }

    if (m_keys[lgck::Key::LAlt] && m_keys[lgck::Key::F4])
    {
        return EVENT_QUIT;
    }

    if (m_keys[lgck::Key::F4])
    {
        m_keys[lgck::Key::F4] = 0;
        flipPause();
    }

    return EVENT_NO_EVENT;
}

void CGame::manageTasks()
{
    int count = m_tasks->getSize();
    for (int i = 0; i < count; ++i)
    {
        CTask *task = (*m_tasks)[i];
        int id;
        if (task->m_ticks < var("ticks"))
        {
            switch (task->m_task)
            {
            case CTask::TASK_SPAWN:
            case CTask::TASK_REBIRTH:
                id = m_sFW->add(task->m_actor);
                (*m_sFW)[id].map();
                break;
            }
            m_tasks->removeAt(i);
            --count;
        }
    }
}

void CGame::updateHP()
{
    if (var("ticks") % 2 == 0)
    {
        if (getDisplayState())
        {
            if (getDisplayAlpha() < (255 - 7))
            {
                setDisplayAlpha(getDisplayAlpha() + 4);
            }
            else
            {
                setDisplayState(false);
            }
        }
        else
        {
            if (getDisplayAlpha())
            {
                setDisplayAlpha(getDisplayAlpha() - 2);
            }
        }
    }
}

int CGame::unfoldEvents()
{
    return call("unfoldEvents1");
}

void CGame::setLookUp(bool lookup)
{
    var("lookup") = lookup ? 1 : 0;
}

bool CGame::getLookUp()
{
    return var("lookup") != 0;
}

void CGame::setLevelGoal(int goal)
{
    var("levelGoal") = goal;
}

int CGame::getLevelGoal()
{
    return var("levelGoal");
}

/////////////////////////////////////////////////////////////////////
// settings

bool CGame::initSettings()
{
    CSettings &settings = *m_settings;
    var("FULLSCREEN") = settings[PARAM_FULLSCREEN].valueInt;
    var("CONST_TIMEOUT") = settings[PARAM_TIMEOUT].valueInt;
    var("AUTO_CENTER_SPEED") = settings[PARAM_AUTO_CENTER_SPEED].valueInt;

    // find points
    if (m_points)
    {
        delete m_points;
        m_points = nullptr;
    }

    if (settings[PARAM_POINTS].value[0] == ':')
    {
        CLuaVM::debugv("points internal");
        FILEWRAP file;
        if (file.open(settings[PARAM_POINTS].value.c_str()))
        {
            m_points = new CFrameSet;
            m_points->setName("points.obl5");
            m_points->read(file);
            file.close();
        }
    }
    else
    {
        CFrameSet *points = m_arrFrames[settings[PARAM_POINTS].value.c_str()];
        if (points)
        {
            m_points = new CFrameSet(points);
        }
    }

    if (!m_points)
    {
        CLuaVM::debugv("`%s` not found", settings[PARAM_POINTS].value.c_str());
        m_lastError = "`points` not found";
        return false;
    }

    var("pointsOBL_frameSet") = m_arrFrames.add(m_points);
    var("pointsOBL_texture") = m_imageManager->add(m_points);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGame::removePointsOBL()
{
    if (var("pointsOBL_texture"))
    {
        m_imageManager->removeAt(var("pointsOBL_texture"));
        var("pointsOBL_texture") = 0;
        m_arrFrames.removeAt(var("pointsOBL_frameSet"));
        var("pointsOBL_frameSet") = 0;
        if (m_points)
        {
            delete m_points;
            m_points = nullptr;
        }
    }
}

void CGame::setVitals(int hp, int lives, int score)
{
    setScore(score);
    setLives(lives);
    counter("start_hp") = hp;
}

bool CGame::initLevelTest(int level, int skill, bool initSound)
{
    // Intialize the game settings
    CLuaVM::debugv("init settings");

    if (!initSettings())
    {
        return false;
    }

    CLuaVM::debugv("init sound");
    // Load sound re/sources
    if (initSound)
    {
        initSounds();
    }

    setSkill(skill);

    CLuaVM::debugv("init level");

    // Initialize the level
    if (!initLevel(level))
    {
        return false;
    }

    setHealth(counter("start_hp"));

    // general stats Init
    postInitLevel();

    return true;
}

void CGame::addToHP(int hp)
{
    CActor &player = getPlayer();
    player.get(lgck::EXTRA_HP) = std::max(0, player.get(lgck::EXTRA_HP) + hp);
    player.get(lgck::EXTRA_HP) = std::min((int)player.get(lgck::EXTRA_HP), (int)MAX_HP);
    setDisplayState(true, std::max(getDisplayAlpha(), 0x80));
    if (!player.get(lgck::EXTRA_HP))
    {
        killPlayer(player);
    }
}

void CGame::killPlayer(CActor &player)
{
    player.kill();
}

bool CGame::isPlatformClass(int classId)
{
    return (classId >= CLASS_WACKER_UP_DOWN) && (classId <= CLASS_PLATFORM_LEFT_RIGHT);
}

bool CGame::isBackgroundClass(int classId)
{
    return classId < CLASS_ANIMATED_THINGS;
}

bool CGame::isEndLevelMeet()
{
    CLevel &level = *m_arrLevels[getLevel()];
    CActor &player = getPlayer();
    int goals = (*m_sFW).countGoals();
    bool result = !goals && !player.isDead();
    if (result)
    {
        if (counter("goalLeft"))
        {
            // if we haven't already send this goal.
            callLvEvent(CLevel::EL_GOAL_COLLECTED);
            callGameEvent(CGameEvents::EG_GOAL_COLLECTED);
        }
        // we confirmed that all the goals were completed
        int trigger = level.getSettingInt(CLevel::SPARAM_TRIGGER);
        if (!wasTriggerCalled() && trigger)
        {
            callTrigger(trigger);
            setTriggerState(true);
        }
        CFrame &frame = *m_arrFrames.getFrame(player);
        switch (var("levelGoal"))
        {
        case CLevel::GOAL_UP:
            result = player.m_nY == 0;
            break;

        case CLevel::GOAL_DOWN:
            result = (BUFFERHEI - player.m_nY) <= frame.m_nHei;
            break;

        case CLevel::GOAL_LEFT:
            result = player.m_nX == 0;
            break;

        case CLevel::GOAL_RIGHT:
            result = (BUFFERLEN - player.m_nX) <= frame.m_nLen;
            break;

        case CLevel::GOAL_NO_COMPLETE:
            result = false;
            break;
        }

        // we're counting the goals again in case the trigger
        // has flipped something
        goals = (*m_sFW).countGoals();
        if (result && !goals)
        {
            callLvEvent(CLevel::EL_LEVEL_COMPLETED);
            callGameEvent(CGameEvents::EG_COMPLETE_LEVEL);
        }
        else
        {
            result = false;
            if (goals)
            {
                setTriggerState(false);
            }
        }
    }
    counter("goalLeft") = goals;
    return result;
}

/////////////////////////////////////////////////////////////////////////////
// Initialization and destruction

void CGame::postInitLevel()
{
    CLuaVM::debugv("postInitLevel\n");

    // clear countdowns
    m_countdowns->removeAll();

    // invoke onCreate event
    callLvEvent(CLevel::EL_CREATE);

    //  invoke prepare Level
    callGameEvent(CGameEvents::EG_PREPARE_LEVEL);

    // if this is a restart call the
    // restart event.
    if (m_snapshot->has_snapshot())
    {
        callGameEvent(CGameEvents::EG_RESTART_LEVEL);
        callLvEvent(CLevel::EL_RESTART);
    }

    CLuaVM::debugv("postInitLevel completed\n");
}

/////////////////////////////////////////////////////////////////////
// latest versions

CInventory *CGame::getInventory(const char *name)
{
    return (*m_inventoryTable)[name ? name : "@player"];
}

void CGame::triggerMouseEvent(int x, int y, int button)
{
    int offsetX = 0;
    int offsetY = 0;
    graphics()->getOffset(offsetX, offsetY);
    x -= offsetX;
    y -= offsetY;
    CScene &scene = (*m_sFW);
    if (x >= 0 && x <= m_screenLen &&
        y >= 0 && x <= m_screenHei)
    {
        int target = scene.whoIs(x + m_mx, y + m_my);
        if (target != INVALID)
        {
            CActor &entry = scene[target];
            switch (button)
            {
            case BUTTON_LEFT:
                entry.callEvent(CObject::EO_LEFTCLICK);
                break;
            case BUTTON_RIGHT:
                entry.callEvent(CObject::EO_RIGHTCLICK);
                break;
            case BUTTON_MIDDLE:
                entry.callEvent(CObject::EO_RIGHTCLICK);
                break;
            }
        }
    }
}

void CGame::stopMusic()
{
    m_music->stop();
}

void CGame::setClosure(int event)
{
    unsigned long long tm;
    tm = microtime();
    tm -= m_startTime;
    counter("closureEvent") = event;
    counter("closureTime") = tm + counter("closureDelay");
}

void CGame::setLastKey(int keyCode)
{
    counter("lastKey") = keyCode;
}

int CGame::testKey(int keyCode)
{
    return m_keys[keyCode];
}

void CGame::setKey(int keyCode, char value)
{
    m_keys[keyCode] = value;
}

void CGame::debug(const char *s)
{
    m_lua.debug(s);
}

CScene &CGame::scene()
{
    return *m_sFW;
}

CMap &CGame::map()
{
    return *m_map;
}

/////////////////////////////////////////////////////////////////////////////
// lua

void CGame::generateRuntimeLua(std::string &s)
{
    // dynamically create and execute runtime.lua
    std::string out;
    readScript("common.lua", out);
    s += out;

    readScript("functions.lua", out);
    s += out;
    s += std::string("-- runtime.lua\n\n-- OBJECT TYPES\n\n");

    char tmp[512];
    for (int n = 0; n < m_arrProto.getSize(); ++n)
    {
        CProto &proto = m_arrProto[n];
        std::string name = proto.m_szName;
        transform(name.begin(), name.end(), name.begin(), upperClean);
        sprintf(tmp, "OBJECT_%-40s = %d;\n", name.c_str(), n);
        s += std::string(tmp);
    }

    for (int n = 0; n < m_arrProto.getSize(); ++n)
    {
        CProto &proto = m_arrProto[n];
        std::string name = proto.m_szName;
        transform(name.begin(), name.end(), name.begin(), upperClean);
        sprintf(tmp, "SPRITE_%-40s = %d;\n", name.c_str(), n);
        s += std::string(tmp);
    }

    s += "\n-- OBJECT CLASSES\n\n";

    for (int n = 0; n < CGame::MAX_CLASSES; ++n)
    {
        if (!m_className[n].empty())
        {
            std::string name = m_className[n];
            transform(name.begin(), name.end(), name.begin(), upperClean);
            sprintf(tmp, "CLASS_%-40s = 0x%.2x;\n", name.c_str(), n);
            s += std::string(tmp);
        }
    }

    s += "\n-- IMAGE SETS\n\n";

    for (int n = 0; n < m_arrFrames.getSize(); ++n)
    {
        std::string name = m_arrFrames[n]->getName();
        transform(name.begin(), name.end(), name.begin(), upperClean);
        sprintf(tmp, "IMAGES_%-40s = 0x%.2x;\n", name.c_str(), n);
        s += std::string(tmp);
    }

    s += "\n";

    typedef struct
    {
        const char *name;
        int value;
    } GLOBAL;

    GLOBAL globals[] = {
        {"BUFFERLEN", BUFFERLEN},
        {"BUFFERHEI", BUFFERHEI},
        {"FULLSCREEN", (int)var("FULLSCREEN")},
        {"CONST_TIMEOUT", (int)var("CONST_TIMEOUT")},
        {"SKILL", static_cast<int>(svar("skill"))},
        {nullptr, 0}};

    s += "-- GENERAL GAME SETTINGS (deprecated)\n";
    s += "-- please use `var_get(\"setting\");`\n\n";

    for (int i = 0; globals[i].name; ++i)
    {
        char t[128];
        sprintf(t, "%-47s = %d\n", globals[i].name, globals[i].value);
        s += t;
    }

    s += std::string("-- EVENT PROCEDURES (OBJECTS)\n\n");
    for (int n = 0; n < m_arrProto.getSize(); ++n)
    {
        CObject &object = m_arrProto.getObject(n);
        for (int j = 0; j < object.getEventCount(); ++j)
        {
            const char *luaCode = object.getEvent(j);
            if (luaCode[0])
            {
                int len = strlen(CProtoArray::getEventName(j)) + strlen(luaCode) + 128;
                auto tmp = new char[len];
                sprintf(tmp, "function event_obj_%d_%s (self, ticks)\n"
                             "%s\n"
                             "end\n\n",
                        n, CProtoArray::getEventName(j), luaCode);
                s += std::string(tmp);
                delete[] tmp;
            }
        }
    }

    s += "\n";
    if (getSize())
    {
        for (int w = 0; w < getSize(); ++w)
        {
            sprintf(tmp, "\n\n-- EVENT PROCEDURES (LEVEL %d)\n\n", w + 1);
            s += std::string(tmp);
            CLevel &level = *(m_arrLevels[w]);
            for (int i = 0; i < level.getEventCount(); ++i)
            {
                if (level.getEvent(i)[0])
                {
                    int len = strlen(level.getEventName(i)) + strlen(level.getEvent(i)) + 128;
                    auto tmp = new char[len];
                    sprintf(tmp, "function event_level_%d_%s()\n"
                                 "%s\n"
                                 "end\n\n",
                            w + 1, level.getEventName(i), level.getEvent(i));
                    s += std::string(tmp);
                    delete[] tmp;
                }
            }
        }
    }

    s += std::string("-- EVENT PROCEDURES (GAME)\n\n");
    for (int i = 0; i < CGameEvents::getSize(); ++i)
    {
        if (m_events->getEvent(i)[0])
        {
            int len = strlen(CGameEvents::getName(i)) + strlen(m_events->getEvent(i)) + 128;
            auto tmp = new char[len];
            sprintf(tmp, "function event_%s()\n"
                         "%s\n"
                         "end\n\n",
                    CGameEvents::getName(i), m_events->getEvent(i));
            s += std::string(tmp);
            delete[] tmp;
        }
    }
}

bool CGame::loadScript(const char *scriptName)
{
    const char fmt[] = "./scripts/%s";
    auto path = new char[strlen(fmt) + strlen(scriptName) + 1];
    sprintf(path, fmt, scriptName);

    FILEWRAP file;
    bool result = file.open(path);
    if (!result)
    {
        CLuaVM::debugv("-- loading internal:%s\n", scriptName);
        sprintf(path, ":/scripts/%s", scriptName);
        result = file.open(path);
    }

    if (result)
    {
        char *script = new char[file.getSize() + 1];
        script[file.getSize()] = 0;
        file.read(script, file.getSize());
        file.close();
        puts(script);
        m_lua.exec(script);

        delete[] script;
    }
    else
    {
        auto tmp = new char[128];
        const char fmt[] = "-- failed to load `%s`\n";
        sprintf(tmp, fmt, scriptName);
        m_lua.debug(tmp);
        CLuaVM::debugv(fmt, tmp);
        delete[] tmp;
    }
    delete[] path;
    return result;
}

bool CGame::readScript(const char *scriptName, std::string &out)
{
    const char fmt[] = "./scripts/%s";
    auto path = new char[strlen(fmt) + strlen(scriptName) + 1];
    sprintf(path, fmt, scriptName);

    FILEWRAP file;
    bool result = file.open(path);
    if (!result)
    {
        CLuaVM::debugv("-- loading internal:%s\n", scriptName);
        sprintf(path, ":/scripts/%s", scriptName);
        result = file.open(path);
    }
    delete[] path;

    if (result)
    {
        char *script = new char[file.getSize() + 1];
        script[file.getSize()] = 0;
        file.read(script, file.getSize());
        file.close();
        out = script;
        delete[] script;
    }
    else
    {
        auto tmp = new char[128];
        const char fmt2[] = "-- failed to load `%s`\n";
        sprintf(tmp, fmt2, scriptName);
        m_lua.debug(tmp);
        CLuaVM::debugv(fmt2, tmp);
        delete[] tmp;
    }
    return result;
}

void CGame::exec(const char *luaCode)
{
    m_lua.exec(luaCode);
}

void CGame::callLvEvent(int eventId)
{
    unsigned int levelId = var32("level");
    CLevel &level = *(m_arrLevels[levelId]);
    const char *luaCode = level.getEvent(eventId);
    if (luaCode[0])
    {
        auto fnName = new char[255];
        sprintf(fnName, "event_level_%d_%s", levelId + 1, CLevel::getEventName(eventId));

        /* the function name */
        lua_getglobal(m_lua.getState(), fnName);

        /* do the call (1 arguments, 0 result) */
        int status = lua_pcall(m_lua.getState(), 0, 0, 0);
        if (status != 0)
        {
            m_lua.reportErrors(status, fnName);
        }
        delete[] fnName;
    }
}

void CGame::callGameEvent(int eventId)
{
    const char *luaCode = m_events->getEvent(eventId);
    if (luaCode[0])
    {
        auto fnName = new char[255];
        sprintf(fnName, "event_%s", CGameEvents::getName(eventId));
        lua_getglobal(m_lua.getState(), fnName);
        int status = lua_pcall(m_lua.getState(), 0, 0, 0);
        if (status != 0)
        {
            m_lua.reportErrors(status, fnName);
        }
        delete[] fnName;
    }
}

void CGame::callObjEvent(int objId, int eventId)
{
    CActor &actor = m_sFW->get(objId);
#ifdef TRACKMSG
    if (eventId != CObject::EO_HANDLER && eventId != CObject::EO_FIRE)
    {
        qDebug("callObjEvent() objId: %d event %d [%s]", objId, eventId, CProtoArray::getEventName(eventId));
        actor.debug();
    }
#endif
    int proto = actor.m_nProto;
    if (proto <= 0)
    {
        CLuaVM::debugv("callObjEvent() invalid call rejected: objId: %d, proto: 0x%.4x, event: 0x%x",
                       objId,
                       proto,
                       eventId);
        return;
    }

    CObject &object = m_arrProto.getObject(actor.m_nProto);
    const char *luaCode = object.getEvent(eventId);
    if (luaCode[0])
    {
        auto fnName = new char[255];
        sprintf(fnName, "event_obj_%d_%s", actor.m_nProto, CProtoArray::getEventName(eventId));
        callStaticHandler(fnName, objId);
        delete[] fnName;
    }
}

void CGame::callStaticHandler(const char *fnName, int objId)
{
    // the function name
    lua_getglobal(m_lua.getState(), fnName);
    // the first argument
    lua_pushnumber(m_lua.getState(), objId);
    // the second argument
    lua_pushnumber(m_lua.getState(), var("ticks"));
    // do the call (2 arguments, 1 result)
    int status = lua_pcall(m_lua.getState(), 2, 0, 0);
    if (status != 0)
    {
        m_lua.reportErrors(status, fnName);
    }
}

int CGame::call(const char *fnName)
{
    lua_State *L = m_lua.getState();
    lua_getglobal(L, fnName);
    int status = lua_pcall(L, 0, 1, 0);
    if (status != 0)
    {
        m_lua.reportErrors(status, fnName);
        lua_pop(L, 1);
        return EVENT_ERROR;
    }
    else
    {
        if (!lua_isnumber(L, -1))
        {
            m_lua.debug("function must return a number");
        }
        int z = lua_tonumber(L, -1);
        lua_pop(L, 1);
        return z;
    }
}

void CGame::error(const char *fnName, int argc)
{
    char tmp[256];
    sprintf(tmp, "-- error: %s(...) requires %d args.",
            fnName, argc);
    m_lua.error(tmp);
}

CGame &CGame::getGame()
{
    return *m_game;
}

void CGame::attach(IImageManager *im)
{
    CLuaVM::debugv(im ? "attaching IImageManager" : "detaching IImageManager");
    m_imageManager = im;
    cacheImages();
}

void CGame::attach(IGraphics *gr)
{
    CLuaVM::debugv(gr ? "attaching IGraphics" : "detaching IGraphics");
    m_graphics = gr;
}

void CGame::attach(IMusic *mu)
{
    CLuaVM::debugv(mu ? "attaching IMusic" : "detaching IMusic");
    m_music = mu;
}

void CGame::attach(ISound *sn)
{
    CLuaVM::debugv(sn ? "attaching ISound" : "detaching ISound");
    m_sound = sn;
}

IGraphics *CGame::graphics()
{
    return m_graphics;
}

IMusic *CGame::music()
{
    return m_music;
}

IDisplayManager *CGame::displays()
{
    if (m_graphics)
    {
        return m_graphics->displayManager();
    }
    else
    {
        return nullptr;
    }
}

int &CGame::counter(const char *s)
{
    return (m_counters)[s];
}

bool CGame::playSound(const char *name)
{
    CSnd *snd = m_arrSounds[name];
    if (snd)
    {
        m_sound->play(snd->getUID());
    }
    else
    {
        CLuaVM::debugv("invalid requested sound: %s", name);
    }
    return snd != nullptr;
}

bool CGame::playSound(int index)
{
    CSnd *snd = m_arrSounds[index];
    if (snd)
    {
        m_sound->play(snd->getUID());
    }
    else
    {
        CLuaVM::debugv("invalid requested sound: %d", index);
    }
    return snd != nullptr;
}

ISound *CGame::sound()
{
    return m_sound;
}

unsigned long long &CGame::var(const char *s)
{
    return m_vars[s];
}

long long &CGame::svar(const char *s)
{
    unsigned long long &t = m_vars[s];
    return *reinterpret_cast<long long *>(&t);
}

unsigned int &CGame::var32(const char *s)
{
    unsigned long long &t = m_vars[s];
    return *reinterpret_cast<unsigned int *>(&t);
}

void CGame::setEngineState(unsigned int state)
{
    unsigned long long now;
    CLevel &level = *m_arrLevels[getLevel()];
    var("engineState") = state;
    switch (state)
    {
    case ES_TIMEOUT:
    case ES_INTRO:
        now = microtime();
        var("timeTo") = now + level.getSettingInt("introTime");
        break;
    case ES_PLAYLEVEL:
        break;
    }
}

int CGame::runEngine()
{
    uint64_t now;
    switch (var("engineState"))
    {
    case ES_TIMEOUT:
    case ES_INTRO:
        updateJoyState();
        now = microtime();
        if ((now >= var("timeTo")) ||
            (getJoyState() & (JOY_JUMP | JOY_FIRE)))
        {
            setEngineState(ES_PLAYLEVEL);
            if (!initLevel(getLevel()))
            {
                return -1;
            }
            // general stats Init
            postInitLevel();
        }
        return CGame::EVENT_NO_EVENT;
    }
    return unfoldEvents();
}

void CGame::updateScreen()
{
    CLevel *level;
    switch (var("engineState"))
    {
    case ES_INTRO:
    case ES_TIMEOUT:
        level = m_arrLevels[getLevel()];
        char text[32];
        graphics()->clear(CGame::bgr2rgb(strtol(level->getSetting("introBkColor"), nullptr, 16)));
        if (svar("WarpTo") == INVALID)
        {
            sprintf(text, "Level %.2d", getLevel() + 1);
        }
        else
        {
            sprintf(text, "Warp to Level %.2d", getLevel() + 1);
        }
        if (var("engineState") == ES_TIMEOUT)
        {
            strncpy(text, "Ran out of time", sizeof(text));
        }
        displays()->drawText(-1, -1, text, 0, 15,
                             CGame::bgr2rgb(strtol(level->getSetting("introTextColor"), nullptr, 16)));
        callLvEvent(CLevel::EL_INTRO_DRAW);
        break;
    default:
        graphics()->drawScreen();
        callLvEvent(CLevel::EL_DRAW);
    }
}

void CGame::saveGame(IFile &file)
{
    // main headers
    file.write("LGCK", 4);
    unsigned int version = getEngineVersion();
    unsigned int uuid = 0;
    unsigned int size;
    file.write(&version, sizeof(version));
    file.write(&uuid, sizeof(uuid));
    file << m_fileName;
    file << m_path;

    // write Actors, bg and layers
    m_sFW->write(file);
    m_sBK->write(file);
    m_layers->write(file);

    // Write Counters
    size = m_counters.size();
    file.write(&size, sizeof(unsigned int));
    for (auto kv : m_counters)
    {
        std::string key = kv.first;
        int val = kv.second;
        file << key;
        file.write(&val, sizeof(int));
    }

    // Write Variables
    var("__mx") = m_mx;
    var("__my") = m_my;
    size = m_vars.size();
    file.write(&size, sizeof(unsigned int));
    for (auto kv : m_vars)
    {
        const std::string key = kv.first;
        unsigned long long val = kv.second;
        file << key;
        file.write(&val, sizeof(unsigned long long));
    }

    // write inventory table
    m_inventoryTable->write(file);

    // save displays
    IDisplayManager &d = *displays();
    size = d.getSize();
    file.write(&size, sizeof(unsigned int));
    for (unsigned int i = 0; i < size; ++i)
    {
        d[i].write(file);
    }

    // save tasks
    m_tasks->write(file);

    // save strv
    size = m_strings.size();
    file.write(&size, sizeof(unsigned int));
    for (auto kv : m_strings)
    {
        const std::string key = kv.first;
        const std::string val = kv.second;
        file << key;
        file << val;
    }

    callGameEvent(CGameEvents::EG_SAVEGAME);

    // TODO: save snapshot
    m_snapshot->write(file);
}

void CGame::loadGame(IFile &file)
{
    // main header
    char signature[4];
    unsigned int version = 0;
    unsigned int uuid = 0;
    unsigned int size;
    file.read(signature, sizeof(signature));
    std::string fileName;
    std::string path;
    file.read(&version, 4);
    assert(version == getEngineVersion());
    file.read(&uuid, 4);
    file >> fileName;
    file >> path;
    // TODO: test for incompatible data here

    // read Actors, bg and layers
    m_sFW->read(file);
    m_sBK->read(file);
    m_layers->read(file);

    // Read Counters
    file.read(&size, sizeof(size));
    m_counters.clear();
    CLuaVM::debugv("Counters: %d", size);
    for (unsigned int i = 0; i < size; ++i)
    {
        std::string key;
        file >> key;
        int val = 0;
        file.read(&val, sizeof(val));
        counter(key.c_str()) = val;
    }

    // Read Variables
    file.read(&size, sizeof(size));
    CLuaVM::debugv("Vars: %d", size);
    m_vars.clear();
    for (unsigned int i = 0; i < size; ++i)
    {
        std::string key;
        file >> key;
        unsigned long long val = 0;
        file.read(&val, sizeof(val));
        var(key.c_str()) = val;
    }

    // Read inventory table
    m_inventoryTable->read(file);

    // fixup the collision map
    // allocate the collision map
    BUFFERLEN = var("BUFFERLEN");
    BUFFERHEI = var("BUFFERHEI");
    m_mx = var("__mx");
    m_my = var("__my");
    Size sx = CMap::size(BUFFERLEN, BUFFERHEI);
    m_map->resize(sx.len, sx.hei);
    // remap all sprites (collision mapping)
    remap();

    m_startTime = var("startTime");

    // load displays
    IDisplayManager &d = *displays();
    d.removeAll();
    file.read(&size, sizeof(unsigned int));
    for (unsigned int i = 0; i < size; ++i)
    {
        CDisplay dx;
        dx.read(file);
        d.add(dx);
    }

    // load tasks
    m_tasks->read(file, version);

    // load strv
    file.read(&size, sizeof(size));
    CLuaVM::debugv("Strings: %d", size);
    m_strings.clear();
    for (unsigned int i = 0; i < size; ++i)
    {
        std::string key;
        file >> key;
        std::string val;
        file >> val;
        strv(key.c_str()) = val.c_str();
    }

    // load snapshot
    m_snapshot->read(file);
    callGameEvent(CGameEvents::EG_LOADGAME);

    // TODO: Fixup internal clock

    // set currLevel
    m_nCurrLevel = var("level");
}

std::string &CGame::strv(const char *s)
{
    return m_strings[s];
}

void CGame::strv_del(const char *s)
{
    m_strings.erase(s);
}

void CGame::var_del(const char *s)
{
    m_vars.erase(s);
}

CSnapshot &CGame::snapshot()
{
    return *m_snapshot;
}

void CGame::remap()
{
    // clear collision map
    m_map->clear();

    // Show invisible systemObject objects
    m_sBK->hideSystemObject(false);
    m_sFW->hideSystemObject(false);

    // Map both background and foreground objects
    m_sBK->map();
    m_sFW->map();

    // Hide invisible systemObject objects
    m_sBK->hideSystemObject(true);
    m_sFW->hideSystemObject(true);
}

bool CGame::initFonts()
{
    CLuaVM::debugv("initFont()");
    CFontManager *fonts = getFonts();
    if (!fonts)
    {
        CLuaVM::debugv("font manager not created");
        return false;
    }

    for (int i = 0; i < fonts->getSize(); ++i)
    {
        CFont *font = fonts->at(i);
        unsigned int tex = font->textureId();
        if (tex)
        {
            CLuaVM::debugv("Font %d already initialized %u", i, tex);
        }
        else
        {
            CLuaVM::debugv("adding font: %d", i);
            int textureId = m_imageManager->add(fonts->nameAt(i), font);
            CLuaVM::debugv("textureId: %u", textureId);
            font->setTextureId(textureId);
        }
    }
    return true;
}

void CGame::setJoyButton(lgck::Button::JoyButton button, char value)
{
    m_buttons[button] = value;
}

const char *CGame::keys()
{
    return m_keys;
}

void CGame::copyDefaultJoyStateMap()
{
    memcpy(&m_joyStateMap, &m_defaultJoyStateMap, sizeof(m_joyStateMap));
}

CGame::JoyStateEntry &CGame::joyStateEntry(int i)
{
    return m_joyStateMap[i];
}

const char *CGame::buttonText(int i)
{
    return i >= 0 ? gamePadButtons[i] : "";
}

int CGame::findButtonText(const char *text)
{
    for (int i = 0; i < lgck::Button::Count; ++i)
    {
        if (strcmp(gamePadButtons[i], text) == 0)
        {
            return i;
        }
    }
    return CGame::INVALID;
}

CLuaVM &CGame::luaVM()
{
    return m_lua;
}

uint64_t CGame::startTime()
{
    return m_startTime;
}

CScene *CGame::_fw()
{
    return m_sFW;
}

CScene *CGame::_bk()
{
    return m_sBK;
}

bool CGame::exportJoyStateMap(IFile &file)
{
    uint32_t version = getEngineVersion();
    file.write(SIGNATURE_JOYSTATEMAP, 4);
    file.write(&version, sizeof(uint32_t));
    file.write(&REVISION_JOYSTATEMAP, sizeof(uint32_t));
    file.write(m_joyStateMap, sizeof(m_joyStateMap));
    return true;
}

bool CGame::importJoyStateMap(IFile &file)
{
    uint32_t version = 0;
    uint32_t revision = -1;
    char signature[4];
    file.read(signature, 4);
    if (memcmp(signature, SIGNATURE_JOYSTATEMAP, 4) == 0)
    {
        file.read(&version, sizeof(int32_t));
        file.read(&revision, sizeof(uint32_t));
        if (version == getEngineVersion())
        {
            file.read(m_joyStateMap, sizeof(m_joyStateMap));
            return true;
        }
        else
        {
            m_lastError = "JoyState version is wrong";
        }
    }
    else
    {
        m_lastError = "JoyState signature is wrong";
    }
    return false;
}

void CGame::resetAllCounters()
{
    m_counters.clear();
}

CCountdown *CGame::countdowns()
{
    return m_countdowns;
}
