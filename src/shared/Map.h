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

#ifndef MAP_H
#define MAP_H

#include <unordered_map>
#include "stdafx.h"

class CFrame;

/////////////////////////////////////////////////////////////////////
// CMapEntry

class CMapEntry {

public:

    CMapEntry();
    ~CMapEntry();

    enum {
        MAX_FW = 4
    };

    uint8_t m_nBkClass;
    uint8_t m_nAcClass;
    uint8_t m_bPlayer;
    uint8_t m_fwCount;
    uint8_t m_nFwClass[MAX_FW];
    uint16_t m_nFwEntry[MAX_FW];
    uint16_t m_nAcEntry;
    void mapFw(uint8_t fwClass, uint16_t fwEntry);
    void unMapFw(uint16_t fwEntry);
    void mapAc(uint8_t acClass, uint16_t acEntry);
    void unMapAc(uint16_t acEntry);
    void getFw(int index, uint8_t & fwClass, uint16_t & fwEntry);
};

/////////////////////////////////////////////////////////////////////
// MapSize and MapPos

typedef struct {
    int len;
    int hei;
} Size;

typedef struct {
    int x;
    int y;
} Pos;

/////////////////////////////////////////////////////////////////////
// CHitData

class CHitData {
public:

    CHitData();
    ~CHitData() {}

    enum {
        MAX_TOUCH       = 8
    };

    enum {
        FLAG_NONE		= 0x0,
        FLAG_METAL		= 0x1,
        FLAG_NOSOUND    = 0x2, //  OBSOLETE
        FLAG_WATER		= 0x4,
        FLAG_DEATH		= 0x8,
        FLAG_HIT		= 0x10,
        FLAG_TELEPORT   = 0x20
    } PLAYER_FLAG;

    enum {
        STATE_NONE		= 0x0,
        STATE_FALL		= 0x1,
        STATE_DEAD		= 0x2,
        STATE_HIT		= 0x4,
        STATE_JUMP		= 0x8,
        STATE_BEGINNING = 0x10,
        STATE_FIRING    = 0x20,
        STATE_LOOKUP    = 0x40
    } PLAYER_STATE;

    void add (const CMapEntry & entry);
    void addBk(const uint8_t bkC );
    void addFw(const uint8_t fwC, const uint16_t fwE ) ;
    void addAc(const uint8_t acC, const uint16_t acE ) ;

    uint8_t bkClass[ MAX_TOUCH ];
    uint8_t fwClass[ MAX_TOUCH ];
    uint8_t acClass[ MAX_TOUCH ];
    uint16_t fwEntry[ MAX_TOUCH ];
    uint16_t acEntry[ MAX_TOUCH ];

    int flags;
    bool player;
    int bkCount;
    int fwCount;
    int acCount;
};

/////////////////////////////////////////////////////////////////////
// CMap

class CMap
{
public:
    CMap(const int sx=0, const int sy=0);
    ~CMap();

    enum {
        GRID = 8
    };

    inline CMapEntry & at(const int x, const int y)
    {
        const Pos p = wMap(x,y);
        // return map at (x,y)
        return m_map [ ((p.y & 0xffff) << 16) + (p.x & 0xffff)];
    }

    inline const Pos wMap(const int x, const int y) const
    {
        Pos p;
        p.x = x;
        p.y = y;

        // normalize the coordonates
        if (p.x < 0) {
            p.x += m_len;
        }

        if (p.y < 0) {
            p.y += m_hei;
        }

        if (p.x >= m_len) {
            p.x %= m_len;
        }

        if (p.y >= m_hei) {
            p.y %= m_hei;
        }

       return p;
    }

    inline const Pos wMap(const Pos mapPos) const
    {
        return wMap(mapPos.x, mapPos.y);
    }

    inline static const Pos toMap(const int screenX, const int screenY)
    {
        Pos p;
        p.x = screenX / GRID;
        p.y = screenY / GRID;
        return p;
    }

    inline static const Pos toMap(const Pos screenPos)
    {
        return toMap(screenPos.x, screenPos.y);
    }

    void resize(const int len, const int hei);
    const Size size() const;
    static const Size size(const CFrame &frame);
    static const Size size(const int px, const int py);
    static const Pos toScreen(const int mapX, const int mapY);
    static const Pos toScreen(const Pos mapPos);
    const Pos wScreen(const int screenX, const int screenY) const;
    const Pos wScreen(const Pos screenPos) const;
    void clear();

protected:
    std::unordered_map <unsigned int, CMapEntry>m_map;
    int m_len;
    int m_hei;
};

#endif // MAP_H
