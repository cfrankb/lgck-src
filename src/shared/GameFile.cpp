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

// Game.cpp : implementation file
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>
#include "FileWrap.h"
#include "Folders.h"
#include "ss_version.h"
#include "GameFile.h"
#include "Selection.h"
#include "Snd.h"
#include "Frame.h"
#include "StringTable.h"
#include "PathBlock.h"
#include "../shared/interfaces/IImageManager.h"
#include "Level.h"
#include "Tasks.h"
#include "../shared/Layer.h"
#include "GameEvents.h"
#include "../shared/helper.h"
#include "displayconfig.h"
#include "fontmanager.h"

CSettings::SETTING CGameFile::m_gameDefaults[] =
{
    { "title", "LGCK runtime", 0 },
    { "icon", ":/images/lgck.png", 0 },
    { "author", "", 0 },
    { "description", "", 0 },
    { "default_speed", "60", 60},
    { "auto_center_granular", "8", 8},
    { "screen_w", "640", 640},
    { "screen_h", "480", 480},
    { "fullscreen", "0", 0},
    { "signs", ":/images/signs.obl5", 0},
    { "points", ":/images/points.obl5", 0},
    { "lookup", "0", 0},
    { "symbols", " abcdefghijklmnopqrstuvwxyz!0123456789$'", 0},
    { "timeout", "240", 240},
    { "#walk_delay", "6", 6},
    { "#fall_speed", "2", 2},
    { "#max_fall", "30", 30},
    { "level_hello", "Level %.2d", 0},
    { "#stretch", "0", 0},
    { "#jump_up",      "00 00 00 00 00 ff 00 00 00 00 00 ff 01 01 01 01 01 ff 01 01 01 01 01", 0},
    { "#jump_down", "", 0},
    { "#jump_left",    "00 00 02 00 00 ff 00 02 02 02 00 ff 01 02 02 02 01 ff 01 01 02 01 01", 0},
    { "#jump_right",   "00 00 03 00 00 ff 00 03 03 03 03 00 ff 01 03 03 03 01 ff 01 01 03 01 01", 0},
    { "#jump_up_left", "00 00 00 00 00 ff 00 00 02 02 02 ff 01 01 02 02 02 02 ff 01 01 01 01 01", 0},
    { "#jump_up_right", "00 00 00 00 00 ff 00 00 03 03 03 ff 01 01 03 03 03 03 ff 01 01 01 01 01", 0},
    { "#jump_down_left", "00 00 02 02 02 ff 00 00 02 02 02 ff 02 02 02 02 02 ff 02 01 01 01 01", 0},
    { "#jump_down_right", "00 00 03 03 03 ff 00 00 03 03 03 ff 03 03 03 03 03 ff 03 01 01 01 01", 0},
    { "auto_center_speed", "4", 4},
    { "lives", "5", 5},
    { "hp", "32", 32},
    { "lives_max", "99", 99},
    { "hp_max", "255", 255},
    { "", "", 0}
};

CGameFile::SOUND CGameFile::m_defaultSounds[] = {
    {"(no sounds)", ""} ,
    {"FALL", "fall.wav"} ,
    {"JUMP", "jump.wav"} ,
    {"METAL", "metal2.wav"} ,
    {"OUCH", "ouchfast.wav"} ,
    {"POUFLIGHT", "pouflght.wav"} ,
    {"SHORTJUMP", "shorjump.wav"} ,
    {"WALK", "walk5.wav"} ,
    {"WALK2", "walk99.wav"} ,
    {"", ""}
};

///////////////////////////////////////////////////////////////////////
// initialization

CGameFile::CGameFile()
{
    m_clipboard = new CSelection();
    char path[PATH_MAX];
    if (realpath("./", path)) {
        m_path = std::string(path) + "/";
    } else {
        m_path = "./";
    }
    m_paths = new CPathBlock;
    m_strings = new CStringTable;
    m_events = new CGameEvents;
    m_settings = new CSettings;
    m_imageManager = NULL;
    m_displayConfig = new CDisplayConfig;
    m_displayConfig->reset();
    m_fontManager = new CFontManager;
    // ImageManager must be created before Init();
    init();
    parseClassList();
}

CGameFile::~CGameFile()
{
    delete m_clipboard;
    delete m_strings;
    delete m_events;
    delete m_settings;
    delete m_displayConfig;
    delete m_fontManager;
}

void CGameFile::parseClassList()
{
    //qDebug("parseClassList()");
    typedef struct
    {
        int id;
        const char *group; // Bk, Ob, Mn, Pl
        const char *name;
        const char *info;
    } CLASS_DEFINITION;

    const CLASS_DEFINITION classes[] =  {
        { 0x00, "BK", "Background decoration",  "Ignored entirely by the engine. They mostly serves as non-moving backdrop elements (e.g. trees, grass, etc.)" },
        { 0x01, "BK", "Antigravity", "Prevents other objects from falling in gravity afflicted levels. This is similar to `Metal ladders` but without the sound." },
        { 0x03, "BK", "Metal Ladder", "This is variation of `Antigravity` which plays a metal sound when walked across." },
        { 0x0a, "BK", "Solid Down Only", "Prevents moving down only. This allows the player to jump through these objects from underneath."},
        { 0x0b, "BK", "Revert to Background", "Used to assign a background attribute to a map location. Mostly used to patch other background class object such as `solid` brick walls."},
        { 0x0d, "BK", "Immediately Deadly", "Player object immediately die when they come into contact with this."},
        { 0x0e, "BK", "Solid Background", "Think of this as walls, bricks and non-moving platforms."},
        { 0x0f, "BK", "Stop", "Prevent monsters from voluntarily moving through. It has no effect on gravity."},
        { 0x10, "BK", "Animated Thing", "Non-moving object which can cycle through all the assigned frames. This is mostly for decorations."},
        { 0x11, "OB", "Pickup Trigger", "When touched by the player will automatically trigger other objects with same trigger tag, defined as recipients."},
        { 0x12, "OB", "Inventory Item", "These objects can be collected and used as secret buddy for doors etc. ex. Keys"},
        { 0x13, "OB", "Open To Owner", "This object will deny anyone except the owner of their secret buddy object. (see features)"},
        { 0x14, "OB", "Teleportation Pad", "Moves the player to another location on the same level. This place is designated through a `teleportation destination` which shares the same trigger key."},
        { 0x15, "OB", "Teleportation Destination", "This is where the teleportation pad leads to. The two must have matching trigger keys."},
        { 0x16, "OB", "Z-Key Switch", "This object will flip recipient objects with the same trigger key by hiding/showing them. "},
        { 0x1d, "OB", "Animate Once", "Similar to animated things only it goes through the `default` animation sequence once and then dies. "},
        { 0x1e, "BK", "Solid Animated", "Similar to animated things, except that it also acts as a solid objects. can be used for wall and such."},
        { 0x1f, "PL", "Player Object", "Designates the player on a given level. any level must include at least one object of this class."},
        { 0x20, "MN", "Generic Monster", "This monster can move up, down, left and right. The algorithm is a modified path find."},
        { 0x21, "MN", "User Defined Monster", "This is a user programmable `Generic Monster`."},
        { 0x22, "MN", "Drone UP/DOWN", "This monster can only move up/down."},
        { 0x23, "MN", "Drone LEFT/RIGHT", "This monster can only move left/right."},
        { 0x24, "MN", "Attacker", "Appears at random along the world borders. Moves in a straight line until it attacks."},
        { 0x27, "MN", "Vampire Plant", "Non-moving monster. Cause damages to nearby players."},
        { 0x2c, "MN", "Whacker UP/DOWN", "Similar to Platform but doesn't carry the player. Platforms and Whackers can also crush players."},
        { 0x2d, "MN", "Whacker LEFT/RIGHT", "Similar to Platform but doesn't carry the player. Platforms and Whackers can also crush players."},
        { 0x2e, "MN", "Platform UP/DOWN", "Platform such a lifts, or move blocks, which can carry player up and down. Platforms and Whackers can also crush players."},
        { 0x2f, "MN", "Platform LEFT/RIGHT", "Platform such a lifts, or move blocks, which can carry player left and right. Platforms and Whackers can also crush players."},
        { 0x40, "OT", "Player Buller", ""},
        { 0x41, "OT", "Monster Bullet", ""}
    };

    for (unsigned int i=0; i < sizeof(classes) / sizeof(CLASS_DEFINITION); ++i){
        const CLASS_DEFINITION & c = classes[i];
        m_className[c.id] = c.name;
        m_classInfo[c.id] = c.info;
    }
}


void CGameFile::init()
{
    setDirty( false );

    m_fileName = "";
    m_nLevels = 0;
    m_arrLevels = NULL;
    m_nCurrLevel = 0;

    // Add the default frames set
    CFrame *pFrame = new CFrame (8,8);
    CFrameSet *pFrameSet = new CFrameSet;
    pFrameSet->setName("(blank)");
    pFrameSet->add(pFrame);
    m_arrFrames.add (pFrameSet);
    //cache().forget();
    if (m_imageManager) {
        cache()->add(pFrameSet);
    }

    // Add the default Proto-object
    m_arrProto.add ( CProto ("(blank)"));

    // reset default settings
    m_settings->copySettings( m_gameDefaults );

    // initialize the default sounds
    initDefaultSounds();
}

void CGameFile::initDefaultSounds()
{
    // sounds

    m_arrSounds.forget();

    for (int i=0; !m_defaultSounds[i].name.empty(); ++i) {
        if (!m_defaultSounds[i].src.empty()) {
            //QString fileName;
            int len = m_defaultSounds[i].src.size() + 16;
            char tmp[len];
            sprintf(tmp, ":/res/%s", m_defaultSounds[i].src.c_str());
            //fileName = tmp;

            CFileWrap file;
            if (file.open(tmp)) {
                char *data = new char[file.getSize()];
                int size = file.getSize();
                file.read( data, size);
                file.close();
                m_arrSounds.add(new CSnd(m_defaultSounds[i].name.c_str(), data, size));
            } else {
                //qDebug("can't find snd: `%s`\n", q2c(fileName));
                m_arrSounds.add(new CSnd(m_defaultSounds[i].name.c_str(), NULL, 0));
            }
        } else {
            m_arrSounds.add(new CSnd(m_defaultSounds[i].name.c_str(), NULL, 0));
        }
    }
}

void CGameFile::forget ()
{
    setDirty( false );

    m_arrFrames.forget();
    m_arrProto.forget();

    if (m_nLevels) {
        while (m_nLevels) {
            delete m_arrLevels[0];
            removeLevel (0);
        }
        delete [] m_arrLevels;
    }

    m_nCurrLevel = 0;
    m_arrSounds.forget();

    // remove game events
    m_events->forget();
    m_clipboard->forget();
    m_strings->forget();
    m_displayConfig->reset();
}

void CGameFile::restoreDefaults()
{
    m_settings->copySettings(m_gameDefaults);
}

/////////////////////////////////////////////////////////////////////////////
// attributs

bool CGameFile::isUntitled ()
{
    return m_fileName.empty();
}

bool CGameFile::isDirty() const
{
    return m_dirty;
}

void CGameFile::setDirty(const bool dirty)
{
    m_dirty = dirty;
}

const char * CGameFile::getFileName()
{
    return m_fileName.c_str();
}

void CGameFile::setFileName(const char* fileName)
{
    m_fileName = fileName;
    char path[PATH_MAX];
    if (realpath(fileName, path)) {        
        const char *dir = dirname(path);
        m_path = std::string(dir) + "/";
    } else {
        m_path = "./";
    }
}

/////////////////////////////////////////////////////////////////////////////
// Levels

void CGameFile::removeLevel(int n)
{
    for (int i = n; i < m_nLevels-1; ++i) {
        m_arrLevels[i] = m_arrLevels[i+1];
    }
    m_nLevels--;
}

void CGameFile::addLevel(CLevel *level)
{
    CLevel **t = new CLevel * [ m_nLevels +1 ];

    if (m_nLevels != 0) {
        for (int i=0; i < m_nLevels; ++i) {
            t[i] = m_arrLevels[i];
        }
    }

    t[m_nLevels] = level;
    if (m_nLevels != 0) {
        delete [] m_arrLevels;
    }

    m_arrLevels = t;
    m_nLevels++;
}

void CGameFile::insertLevel (int n, CLevel *level)
{
    addLevel (NULL);

    for (int i=m_nLevels-1; i > n ; --i) {
        m_arrLevels [i] = m_arrLevels [i-1];
    }

    m_arrLevels[n] = level;
}

CLevel * CGameFile::operator [] (int n)
{
    return m_arrLevels[n];
}

CLevel * CGameFile::getLevel(int n)
{
    return m_arrLevels[n];
}

int CGameFile::whoIs(CLevel & level, int x, int y)
{
    int nTarget = -1;

    CLayer *layer = level.getCurrentLayer();
    if (!layer) {
        qDebug("layer is null\n");
    } else {
        for (int n=0; n < layer->getSize(); ++n) {
            CLevelEntry &entry = (*layer)[n];
            CFrameSet & frameSet = * m_arrFrames[entry.m_nFrameSet];
            CFrame * pFrame = frameSet[entry.m_nFrameNo];
            if ((entry.m_nX<= x) && (entry.m_nY<= y) &&
                (entry.m_nX+ pFrame->m_nLen) > x &&
                (entry.m_nY+ pFrame->m_nHei) > y )
                nTarget = n;
        }
    }
    return nTarget;
}

int CGameFile::getSize()
{
    return m_nLevels;
}

const char *CGameFile::getLastError() const
{
    return m_lastError.c_str();
}

/////////////////////////////////////////////////////////////////////////////
// Object sets & proto

void CGameFile::killFrameSet(int nFrameSet)
{
    if ((nFrameSet <= 0) ||
        (nFrameSet >= m_arrFrames.getSize()))
        return;

    for (int i = 0; i < getSize(); ++i) {
        m_arrLevels[i]->killFrameSet(nFrameSet);
    }

    m_arrProto.killFrameSet(nFrameSet);
    m_clipboard->clear();
    m_displayConfig->killFrameSet(nFrameSet);
}

void CGameFile::killProto(int nProto)
{
    if ((nProto <= 0) ||
        (nProto >= m_arrProto.getSize()))
        return;
    for (int i = 0; i < getSize(); ++i) {
        m_arrLevels[i]->killProto (nProto);
    }
    m_arrProto.killProto (nProto);
    m_clipboard->clear();
}

/////////////////////////////////////////////////////////////////////////////
// file i/o

bool CGameFile::read(const char *filepath)
{
    qDebug("reading ...\n");
    CFolders fs;
    if (fs.open(filepath ? filepath:m_fileName.c_str(), false)) {
        forget();
        CFileWrap & file = fs.getFile();

        // version.dat

        VERSION version;
        memset(&version, 0, sizeof(VERSION));

        CFolder::CFileEntry * version_dat = fs.checkOut("version.dat");
        if (version_dat) {
            file.seek(version_dat->getOffset());
            file.read(&version, sizeof(VERSION));
        }

        if (version.id > ((unsigned)SS_LGCK_VERSION)) {
            qDebug("Unsupported version of LGCK: 0x%.8x\n", version.id);            
            const char msg[] = "Unsupported version of LGCK: 0x%.8x\nYou'll need to upgrade to use this file!";
            int len = strlen(msg) + 16;
            char tmp[len];
            sprintf(tmp, msg, version.id);
            m_lastError = tmp;
            return false;
        } else {
            qDebug("Version: 0x%.8x\n", version.id);
        }

        if (!version.game_uid) {
            version.game_uid = ::rand();
        }

        // OBL5 Data

        CFolder *obl = fs["obldata"];
        if (!obl) {
            qDebug("missing `obldata` folder\n");
            m_lastError = "damaged database:\nmissing `obldata` folder";
            return false;
        }

        m_arrFrames.forget();
        for (int i=0; i < obl->getSize(); ++i){
            CFolder::CFileEntry * entry = (*obl)[i];
            file.seek(entry->getOffset());
            CFrameSet *pFrameSet = new CFrameSet;
            pFrameSet->setName(entry->getName());
            if (!pFrameSet->read(file)) {
                m_lastError = "failed to read FrameSet";
                return false;
            }
            m_arrFrames.add(pFrameSet);
        }

        // Proto
        CFolder::CFileEntry * proto = fs.checkOut("proto.dat");
        if (!proto) {
            qDebug("missing `./proto.dat`\n");
            m_lastError = "damaged database:\nmissing `./proto.dat`";
            return false;
        }

        file.seek(proto->getOffset());
        if (!m_arrProto.read(file)) {
            return false;
        }

        // scripts
        CFolder *levels = fs["scripts"];
        if (!levels) {
            qDebug("missing `scripts` folder\n");
            m_lastError = "damaged database:\nmissing `scripts` folder";
            return false;
        }

        for (int i=0; i < levels->getSize(); ++i){
            CFolder::CFileEntry * entry = (*levels)[i];
            file.seek(entry->getOffset());
            CLevel *level = new CLevel;
            if (!level->read(file)) {
                return false;
            }
            addLevel(level);
        }

        // sounds
        CFolder *sounds = fs["sounds"];
        if (!sounds) {
            qDebug("missing `sounds` folder\n");
            m_lastError = "damaged database:\nmissing `sounds` folder";
            return false;
        }

        if (sounds->getSize() == 1) {
            initDefaultSounds();
        } else {
            for (int i = 0; i < sounds->getSize(); ++i){
                CFolder::CFileEntry * entry = (*sounds)[i];
                file.seek(entry->getOffset());
                char *data = new char [  entry->getSize() ];
                file.read( data, entry->getSize());
                CSnd * snd = new CSnd(entry->getName(), data, entry->getSize());
             //   qDebug("Sound:%s", entry->getName());
                m_arrSounds.add(snd);
            }
        }

        // game_settings
        m_settings->copySettings(m_gameDefaults);
        CFolder::CFileEntry * lgck_cfg = fs.checkOut("lgck.cfg");
        if (!lgck_cfg) {
            qDebug("missing `./lgck.cfg`\n");
            m_lastError = "missing `./lgck.cfg`";
            return false;
        }

        file.seek(lgck_cfg->getOffset());
        char *t = new char[lgck_cfg->getSize() + 1];
        file.read(t, lgck_cfg->getSize());
        t[lgck_cfg->getSize()] = 0;
        m_settings->parseSettings(t);
        delete [] t;

        // events
        CFolder::CFileEntry * events = fs.checkOut("events");
        if (events) {
            file.seek(events->getOffset());
            m_events->read(file);
        } else {
            // clear the event array to prevent spill over
            m_events->forget();
            qDebug("no game events defined\n");
        }

        // strings
        CFolder::CFileEntry * strings = fs.checkOut("strings");
        if (strings) {
            file.seek(strings->getOffset());
            m_strings->read(file);
        } else {
            // clear the string array to prevent spill over
            m_strings->forget();
            qDebug("no strings defined\n");
        }

        // paths
        CFolder::CFileEntry * paths = fs.checkOut("paths");
        if (paths) {
            file.seek(paths->getOffset());
            m_paths->read(file);
        } else {
            // clear the string array to prevent spill over
            m_paths->forget();
            qDebug("no paths defined\n");
        }

        // displayconf
        CFolder::CFileEntry * displayConf = fs.checkOut("displayConf.dat");
        if (displayConf) {
            file.seek(displayConf->getOffset());
            m_displayConfig->read(file);
        } else {
            qDebug("Warning: missing `displayConf`. Using defaults.\n");
            m_displayConfig->reset();
        }

        // fonts
        CFolder::CFileEntry * fonts = fs.checkOut("fonts");
        if (fonts) {
            file.seek(fonts->getOffset());
            m_fontManager->read(file);
        } else {
            qDebug("Warning: missing `fonts`. Using defaults.\n");
            m_fontManager->reset();
        }

        qDebug("read done ;) \n");
        return true;
    }

    const char *msg = "can't open `%s`";
    qDebug(msg, m_fileName.c_str());
    char tmp[strlen(msg) + m_fileName.length() + 1];
    sprintf(tmp, msg, m_fileName.c_str());
    m_lastError = tmp;//QString("can't open `%1`").arg(m_fileName);
    return false;
}

bool CGameFile::write(const char *filepath)
{
    //qDebug("Writing to `%s`\n", q2c(m_fileName));
    CFolders fs;
    if (fs.open(filepath ? filepath: m_fileName.c_str(), true)) {
        CFileWrap & file = fs.getFile();
        CFolder & root = fs.addFolder("");
        int aSize, bSize;

        // proto.dat
        fs.writeFile(file, m_arrProto, root, "proto.dat");

        // lgck.cfg
        std::string t;
        m_settings->outputSettings(t);
        file.seek(fs.getSize());
        file.write(t.c_str(), t.length());
        root.addFile("lgck.cfg", fs.getSize(), t.length());
        fs += t.length();

        // events
        fs.writeFile(file, *m_events, root, "events");

        // strings
        fs.writeFile(file, *m_strings, root, "strings");

        // paths
        fs.writeFile(file, *m_paths, root, "paths");

        // version.dat
        VERSION version;
        memset(&version, 0, sizeof(VERSION));
        version.id = SS_LGCK_VERSION;
        version.game_uid = ::rand();

        bSize = fs.getSize();
        file.seek(bSize);
        file.write(&version, sizeof(VERSION));
        aSize = file.getSize();
        root.addFile("version.dat", bSize, aSize - bSize);
        fs += (aSize - bSize);

        CFolder & snd = fs.addFolder("sounds");
        for (int i=0; i < m_arrSounds.getSize(); ++i){
            CSnd * noise = m_arrSounds[i];
            fs.writeFile(file, *noise, snd, noise->getName());
        }

        CFolder & obl = fs.addFolder("obldata");
        for (int i=0; i < m_arrFrames.getSize(); ++i) {
            CFrameSet * filter = m_arrFrames[i];
            fs.writeFile(file, *filter, obl, filter->getName());
        }

        CFolder & levels = fs.addFolder("scripts");
        for (int i = 0; i < getSize(); ++i) {
            char levelName[16];
            sprintf(levelName, "level%d", i + 1);
            fs.writeFile(file, *(m_arrLevels[i]), levels, levelName);
        }

        // displayconf
        fs.writeFile(file, *m_displayConfig, root, "displayConf.dat");

        // fonts
        fs.writeFile(file, *m_fontManager, root, "fonts");

        fs.writeFileIndex ( );
        fs.writeFolderIndex( );
        fs.writeHeader( );

//      qDebug("writing done\n");
        return true;
    }

    //qDebug("writing to file failed\n");
    m_lastError = "writing to file failed";
    return false;
}

unsigned int CGameFile::getVersion()
{
    return SS_LGCK_VERSION;
}

CSelection* CGameFile::getClipboard()
{
    return m_clipboard;
}

CStringTable * CGameFile::getStringTable()
{
    return m_strings;
}

CPathBlock * CGameFile::getPaths()
{
    return m_paths;
}

CGameEvents * CGameFile::getEvents()
{
    return m_events;
}

CSettings * CGameFile::getSettings()
{
    return m_settings;
}

IImageManager *CGameFile::cache()
{
    return m_imageManager;
}

CONST_DATA * CGameFile::getSpriteList()
{
    CONST_DATA * data = new CONST_DATA[m_arrProto.getSize()+1];
    for (int n = 0; n < m_arrProto.getSize(); ++n) {
        const CProto & proto = m_arrProto[n];
        char name[strlen(proto.m_szName)+1];
        strcpy(name, proto.m_szName);
        toUpper(name);
        for (unsigned int i = 0; i < strlen(name); ++i) {
            if (!isalnum(name[i])) {
                name[i] = '_';
            }
        }
        data[n].name = proto.m_szName;
        data[n].value = "SPRITE_" + std::string(name);
    }
    return data;
}

CONST_DATA * CGameFile::getClassList()
{
    int count=0;
    for (int n = 0; n < CGameFile::MAX_CLASSES; ++n) {
        if (!m_className[n].empty()) {
            ++count;
        }
    }

    CONST_DATA * data = new CONST_DATA[count+1];
    for (int n = 0, i =0; n < CGameFile::MAX_CLASSES; ++n) {
        if (!m_className[n].empty()) {
            const char *r = m_className[n].c_str();
            char name[strlen(r)+1];
            strcpy(name, r);
            toUpper(name);
            for (unsigned int i = 0; i < strlen(name); ++i) {
                if (!isalnum(name[i])) {
                    name[i] = '_';
                }
            }
            data[i].name = r;
            data[i].value = "CLASS_" + std::string(name);
            ++i;
        }
    }
    return data;
}

CONST_DATA * CGameFile::getImageList()
{
    CONST_DATA * data = new CONST_DATA[m_arrFrames.getSize()+1];
    for (int n=0; n < m_arrFrames.getSize(); ++n) {
        const char *r = m_arrFrames[n]->getName();
        char name[strlen(r)+1];
        strcpy(name, r);
        toUpper(name);
        for (unsigned int i = 0; i < strlen(name); ++i) {
            if (!isalnum(name[i])) {
                name[i] = '_';
            }
        }
        data[n].name = r;
        data[n].value = "IMAGES_" + std::string(name);
    }
    return data;
}

CONST_DATA * CGameFile::getSoundList()
{
    CONST_DATA * data = new CONST_DATA[m_arrSounds.getSize()+1];
    for (int n=0; n < m_arrSounds.getSize(); ++n) {
        const char *r = m_arrSounds[n]->getName();
        data[n].name = r;
        data[n].value = r;
    }
    return data;
}

int CGameFile::getLevelByUUID(const char *uuid)
{
    for (int i=0; i < m_nLevels; ++i) {
        CLevel *level = m_arrLevels[i];
        if (std::string(level->getSetting("uuid")) == uuid) {
            return i;
        }
    }
    return -1;
}

CDisplayConfig * CGameFile::getDisplayConfig()
{
    return m_displayConfig;
}

int *CGameFile::countFrameSetUses()
{;
    int count = m_arrFrames.getSize();
    int * countFrameSetUses = new int[count];
    memset(countFrameSetUses, 0, sizeof(int)*count);
    CProtoArray & protos = m_arrProto;
    for (int i=0; i < protos.getSize(); ++i) {
        CProto & proto = protos[i];
        ++countFrameSetUses[proto.m_nFrameSet];
    }
    return countFrameSetUses;
}

CFontManager * CGameFile::getFonts()
{
    return m_fontManager;
}
