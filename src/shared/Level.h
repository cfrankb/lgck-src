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

#ifndef _LEVEL_H
#define _LEVEL_H

#include "Settings.h"
#include <string>
#include "ISerial.h"

class IFile;
class CLayer;

// Level.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLevel

class CLevel: public ISerial
{
public:
    CLevel();

public:
    int getSize();
    int getEventCount()
    {
        return m_eventCount;
    }

    static const char * getEventName(int i)
    {
        return m_eventList[i].c_str();
    }

    const char * getEvent(int i)
    {
        return m_events[i].c_str();
    }

    void setEvent(int i, const char * s)
    {
        m_events[i] = s;
    }

    int addLayer(CLayer *layer);
    void insertAt(int i, CLayer *layer);
    CLayer * getMainLayer();
    int getMainLayerId();
    CLayer * getCurrentLayer();
    void setCurrentLayerById(int currLayer);
    int getCurrentLayerById();
    CLayer* removeLayerById(int layerId);
    void forget();
    CLayer & operator [] (int n);
    void moveBy (int tx, int ty);
    void killProto(int nProto);
    void killFrameSet (int nFrameSet);
    CLevel & operator = (CLevel & source);
    void parseSettings(const char * s);
    void outputSettings(std::string & s);
    const char* getSetting(const char *param);
    int getSettingInt(const char * param);
    const char* getSetting(const int index);
    int getSettingInt (const int index);
    void setSetting(const char * param, const char * value, int mask=0xffffffff);
    CSettings & getSettings();
    void debug();
    ~CLevel();
    bool read(IFile & file);
    bool write(IFile & file);
    int m_currLayer;
    int m_mx;
    int m_my;
    void copyEvents(CLevel & src);

    enum {
        SPARAM_TITLE            = 0,
        SPARAM_BKCOLOR          = 1,
        SPARAM_HINT             = 2,
        SPARAM_GOAL             = 3,
        SPARAM_TIME             = 4,
        SPARAM_TRIGGER          = 5,
        SPARAM_WRAP             = 6,
        SPARAM_NO_GRAVITY       = 7,
        SPARAM_TICK_RATE        = 8,
        SPARAM_MUSIC            = 9,
        SPARAM_CLOSURE          = 10,
        SPARAM_UUID             = 11
    } PARAM_LEVEL;

    enum {
        WRAP_NONE               = 0,
        WRAP_UP                 = 1,
        WRAP_DOWN               = 2,
        WRAP_LEFT               = 4,
        WRAP_RIGHT              = 8,
        OPENTOP                 = 16
    } WRAP;

    enum {
        EL_CREATE,
        EL_DRAW,
        EL_RESTART,
        EL_GOAL_COLLECTED,
        EL_LEVEL_COMPLETED,
        EL_TIMEOUT,
        EL_KEY_PRESSED,
        EL_KEY_UP,
        EL_GOAL_KILLED,
        EL_HANDLER,
        EL_NOTIFYCLOSURE,
        EL_INTRO_DRAW
    };

    enum {
        GOAL_ONLY               = 0,
        GOAL_UP                 = 1,
        GOAL_DOWN               = 2,
        GOAL_LEFT               = 3,
        GOAL_RIGHT              = 4,
        GOAL_NO_COMPLETE        = 5
    } GOAL;
    void updateLayerTypes();

protected:
    static std::string m_eventList[];
    int m_eventCount;
    std::string *m_events;
    void writeEvents(IFile & file);
    void readEvents(IFile & file);
    bool readLegacyV1(IFile &file);
    bool readLegacyV2(IFile & file);
    bool readV4(IFile &file, bool compr);
    static CSettings::SETTING m_defaults [];
    CSettings m_settings;
    bool m_dirty;
    int m_size;
    int m_max;
    CLayer **m_layers;
    uint64_t m_guid;
    int getVersion();
    enum {
        GROWBY = 1,
        COMPR = -1,
        UNCOMPR = 0
    };
};

/////////////////////////////////////////////////////////////////////////////

#endif
