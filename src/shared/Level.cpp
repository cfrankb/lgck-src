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

// Level.cpp : implementation file
//

#include <stdlib.h>
#include <cstdio>
#include <stdint.h>
#include <zlib.h>
#include "Level.h"
#include "LevelEntry.h"
#include "Layer.h"
#include "helper.h"
#include "LuaVM.h"
#include "IFile.h"
#include "win32patch.h"

/////////////////////////////////////////////////////////////////////////////
// CLevel

std::string CLevel::m_eventList[] = {
    "onCreate",
    "onDraw",
    "onRestart",
    "onGoalCollected",
    "onLevelCompleted",
    "onTimeOut",
    "onKeyPressed",
    "onKeyUp",
    "onGoalKilled",
    "onHandler",
    "onNotifyClosure",
    "onIntroDraw"};

CSettings::SETTING CLevel::m_defaults[] =
    {
        {"title", "", 0}, // 0
        {"bkcolor", "0040ff", 0x0040ff},
        {"hint", "", 0},
        {"goal", "0", 0}, // 3
        {"time", "0", 0},
        {"trigger", "0", 0},
        {"wrap", "0", 0}, // 6
        {"no_gravity", "0", 0},
        {"tick_rate", "90", 90},
        {"music", "", 0},
        {"closure", "500", 500},
        {"uuid", "", 0},
        {"borderColor", "000000", 0}, // 12
        {"introBkColor", "000000", 0},
        {"introTextColor", "ffffff", 0},
        {"introTime", "1500", 1500},
        {"width", "0", 0},
        {"height", "0", 0},
        {"colorMod", "ffffffff", 0},
        {"author", "", 0}, // 19
        {"", "", 0}};

CLevel::CLevel()
{
    m_max = GROWBY;
    m_size = 0;
    m_layers = new CLayer *[m_max];

    m_mx = 0;
    m_my = 0;

    m_settings.copySettings(m_defaults);
    char *uuid = getUUID();
    m_settings["uuid"].value = uuid;
    delete[] uuid;
    m_eventCount = sizeof(m_eventList) / sizeof(std::string);
    m_events = new std::string[m_eventCount];

    CLayer *layer = new CLayer(CLayer::LAYER_MAIN);
    layer->setName("main layer");
    addLayer(layer);
    m_currLayer = 0;
}

CLevel::~CLevel()
{
    forget();
    if (m_layers)
    {
        delete[] m_layers;
    }

    if (m_events)
    {
        delete[] m_events;
    }
}

bool CLevel::readLegacyV1(IFile &file)
{
    // legacy "level" w/o layer support
    int size = 0;
    int entrySize = 0;
    ulong totalSize = 0;
    ulong compressSize = 0;

    file.read(&size, sizeof(size));
    std::string t;
    m_settings << file;
    file >> t; // dummy marker (used to be description)

    uint8_t t1;                 // dummy marker
    file.read(&t1, sizeof(t1)); // used to be color palette index
    file.read(&entrySize, sizeof(entrySize));
    file.read(&totalSize, 4);
    file.read(&compressSize, 4);

    uint8_t *pCompressData = new uint8_t[compressSize];

    if (totalSize != (size * sizeof(CLevelEntry)))
    {
        CLuaVM::debugv("CLevel() : total uncompressed size doesn't match array size\n");
        return false;
    }

    CLevelEntry *entries = new CLevelEntry[size];
    file.read(pCompressData, compressSize);
    int err = uncompress((uint8_t *)entries,
                         &totalSize,
                         pCompressData,
                         compressSize);

    if (err)
    {
        CLuaVM::debugv("CLevel::Read err=%d\n", err);
    }

    CLayer *layer = new CLayer(CLayer::LAYER_MAIN);
    layer->setName("main layer");
    for (int i = 0; i < size; ++i)
    {
        layer->add(entries[i]);
    }
    addLayer(layer);
    m_currLayer = 0;
    delete[] pCompressData;
    delete[] entries;
    return true;
}

bool CLevel::readLegacyV2(IFile &file)
{
    // TODO: implement the new loading code
    m_settings << file; // load settings
    int size = 0;
    file.read(&size, 4); // number of layers

    CLayer **layers = new CLayer *[size];
    m_currLayer = 0;

    for (int i = 0; i < size; ++i)
    {
        // CLayer *layer = new CLayer;
        layers[i] = new CLayer;
        layers[i]->read(file);
        // addLayer(layer);

        // TODO: find main layer index
        if (layers[i]->getType() == CLayer::LAYER_MAIN)
        {
            m_currLayer = i;
        }

        if (layers[i]->getType() == CLayer::LAYER_BK)
        {
            // add bklayer
            addLayer(layers[i]);
        }
    }

    // add mainLayer
    addLayer(layers[m_currLayer]);
    // set currLayer to main Layer
    m_currLayer = m_size - 1;

    for (int i = 0; i < size; ++i)
    {
        if (layers[i]->getType() == CLayer::LAYER_FW)
        {
            // add fw
            // layers[i]->setType(CLayer::LAYER_BK);
            addLayer(layers[i]);
        }
    }

    delete[] layers;
    return true;
}

bool CLevel::readV4(IFile &file, bool compr)
{
    // TODO: implement the new loading code
    m_settings << file; // load settings
    int size = 0;
    file.read(&size, 4); // number of layers
    m_currLayer = 0;
    for (int i = 0; i < size; ++i)
    {
        CLayer *layer = new CLayer;
        if (!layer->read(file, compr))
        {
            return false;
        }
        addLayer(layer);
    }

    return true;
}

bool CLevel::read(IFile &file)
{
    int version = getVersion();
    forget();
    int marker;
    file.read(&marker, sizeof(marker)); // compr marker
    file.read(&version, sizeof(version));
    bool result = false;
    bool compr = (marker == COMPR);

    switch (version)
    {
    case 0:
    case 1:
    case 2:
        result = readLegacyV1(file);
        break;
    case 3:
        result = readLegacyV2(file);
        break;
    default: // 4+
        result = readV4(file, compr);
    }

    if (m_size > 1)
    {
        for (int i = 0; i < m_size; ++i)
        {
            if (m_layers[i]->getType() == CLayer::LAYER_MAIN)
            {
                m_currLayer = i;
                break;
            }
        }
    }

    if (result)
    {
        readEvents(file);
    }

    return result;
}

void CLevel::insertAt(int i, CLayer *layer)
{
    addLayer(nullptr);
    for (int j = m_size - 1; j > i; --j)
    {
        m_layers[j] = m_layers[j - 1];
    }
    m_layers[i] = layer;
}

int CLevel::addLayer(CLayer *layer)
{
    m_layers[m_size] = layer;
    ++m_size;

    if (m_size == m_max)
    {
        m_max += GROWBY;
        CLayer **t = new CLayer *[m_max];
        for (int i = 0; i < m_size; ++i)
        {
            t[i] = m_layers[i];
        }

        delete[] m_layers;
        m_layers = t;
    }

    return m_size - 1;
}

CLayer *CLevel::getMainLayer()
{
    for (int i = 0; i < m_size; ++i)
    {
        if (m_layers[i]->getType() == CLayer::LAYER_MAIN)
        {
            return m_layers[i];
        }
    }

    return nullptr;
}

int CLevel::getMainLayerId()
{
    for (int i = 0; i < m_size; ++i)
    {
        if (m_layers[i]->getType() == CLayer::LAYER_MAIN)
        {
            return i;
        }
    }

    return -1;
}

CLayer *CLevel::getCurrentLayer()
{
    return m_layers[m_currLayer];
}

void CLevel::setCurrentLayerById(int currLayer)
{
    m_currLayer = currLayer;
}

int CLevel::getCurrentLayerById()
{
    return m_currLayer;
}

CLayer *CLevel::removeLayerById(int layerId)
{
    CLayer *layer = m_layers[layerId];

    for (int i = layerId; i < m_size - 1; ++i)
    {

        m_layers[i] = m_layers[i + 1];
    }

    --m_size;
    if (m_currLayer >= m_size)
    {
        --m_currLayer;
    }

    return layer;
}

bool CLevel::write(IFile &file)
{
    bool compr = true;
    int version = getVersion();
    int marker = COMPR;
    if (!compr)
    {
        marker = UNCOMPR;
    }

    file.write(&marker, 4);  // compr marker
    file.write(&version, 4); // level version

    m_settings >> file; // save settings

    int type = CLayer::LAYER_BK;
    file.write(&m_size, 4); // number of layers
    for (int i = 0; i < m_size; ++i)
    {
        if (m_layers[i]->getType() == CLayer::LAYER_MAIN)
        {
            type = CLayer::LAYER_FW;
        }
        else
        {
            m_layers[i]->setType(type);
        }
        m_layers[i]->write(file, compr);
    }

    writeEvents(file); // write events

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// CLevel message handlers

void CLevel::forget()
{
    m_mx = 0;
    m_my = 0;
    m_size = 0;
    m_currLayer = 0;

    // TODO: delete the layers
}

CLayer &CLevel::operator[](int n)
{
    return *(m_layers[n]);
}

int CLevel::getSize()
{
    return m_size;
}

void CLevel::moveBy(int tx, int ty)
{
    m_mx += tx;
    m_my += ty;
}

void CLevel::killFrameSet(int nFrameSet)
{
    // TODO: redo this function
    for (int i = 0; i < getSize(); ++i)
    {
        m_layers[i]->killFrameSet(nFrameSet);
    }
}

void CLevel::killProto(int nProto)
{
    // TODO: redo this function
    for (int i = 0; i < getSize(); ++i)
    {
        m_layers[i]->killProto(nProto);
    }
}

CLevel &CLevel::operator=(CLevel &s)
{
    // TODO: redo this function

    if (m_layers)
    {
        delete[] m_layers;
        m_layers = nullptr;
    }

    m_mx = s.m_mx;
    m_my = s.m_my;
    m_settings = s.getSettings();

    m_size = s.getSize();
    m_max = m_size + GROWBY;

    m_layers = new CLayer *[m_max];
    for (int i = 0; i < m_size; ++i)
    {
        m_layers[i] = new CLayer(s[i].getType());
        (*(m_layers[i])) = s[i];
    }

    m_currLayer = 0; // TODO: check this layer
    copyEvents(s);
    return *this;
}

const char *CLevel::getSetting(const char *param)
{
    return m_settings[param].value.c_str();
}

int CLevel::getSettingInt(const char *param)
{
    return m_settings[param].valueInt;
}

void CLevel::setSetting(const char *param, const char *value, int mask)
{
    m_settings[param].value = value;
    m_settings[param].valueInt = strtol(value, nullptr, 10) & mask;
}

CSettings &CLevel::getSettings()
{
    return m_settings;
}

void CLevel::writeEvents(IFile &file)
{
    file << m_eventCount;
    for (int i = 0; i < m_eventCount; ++i)
    {
        file << m_eventList[i];
        file << m_events[i];
    }
}

void CLevel::readEvents(IFile &file)
{
    auto masterList = new std::string[getEventCount()];
    for (int i = 0; i < getEventCount(); ++i)
    {
        masterList[i] = m_eventList[i];
        m_events[i] = "";
    }
    int count;
    file >> count;
    for (int i = 0; i < count; ++i)
    {
        std::string eventName;
        std::string eventCode;
        file >> eventName;
        file >> eventCode;
        for (int j = 0; i < getEventCount(); ++j)
        {
            if (masterList[j] == eventName)
            {
                m_events[i] = eventCode;
                break;
            }
        }
    }
    delete[] masterList;
}

void CLevel::copyEvents(CLevel &src)
{
    for (int i = 0; i < m_eventCount; ++i)
    {
        m_events[i] = src.getEvent(i);
    }
}

int CLevel::getVersion()
{
    return 0x00000004;
}

void CLevel::updateLayerTypes()
{
    int mainId = getMainLayerId();
    for (int i = 0; i < m_size; ++i)
    {
        if (i < mainId)
        {
            m_layers[i]->setType(CLayer::LAYER_BK);
        }
        else
        {
            if (i > mainId)
            {
                m_layers[i]->setType(CLayer::LAYER_FW);
            }
        }
    }
}

const char *CLevel::getSetting(const int index)
{
    return m_settings[index].value.c_str();
}

int CLevel::getSettingInt(const int index)
{
    return m_settings[index].valueInt;
}

void CLevel::outputSettings(std::string &s)
{
    m_settings.outputSettings(s);
}
