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

#include "Map.h"
#include "vlamits3.h"
#include "Frame.h"
#include <cstring>
#include <cstdio>

/////////////////////////////////////////////////////////////////////
// CHitData()

CHitData::CHitData() {
    memset(this, 0, sizeof(CHitData));
}

void CHitData::add(const CMapEntry & entry) {
    if (entry.m_nBkClass) {
        addBk(entry.m_nBkClass);
    }
    for (int i=0; i < entry.m_fwCount; ++i) {
        addFw(entry.m_nFwClass[i],entry.m_nFwEntry[i]);
    }
    if (entry.m_nAcClass) {
        addAc(entry.m_nAcClass, entry.m_nAcEntry);
    }
    if (entry.m_bPlayer) {
        player = true;
    }
}

void CHitData::addBk(const uint8_t bkC) {
    if (bkCount < MAX_TOUCH) {
        int i;
        for (i = 0; i < bkCount; ++i) {
            if (bkClass[i] == bkC) {
                return;
            }
        }

        if (i == bkCount) {
            bkClass [ bkCount] = bkC;
            ++bkCount;

            switch ( bkC ) {
            case CLASS_WATER:
                flags |= FLAG_NOSOUND | FLAG_WATER;
                break;

            case CLASS_METAL_LADDERS:
                flags |= FLAG_METAL;
                break;

            case CLASS_IMMEDIATE_DEADLY:
                flags |= FLAG_DEATH;
            }
        }
    }
}

void CHitData::addFw(const uint8_t fwC,const uint16_t fwE) {
    if (fwCount < MAX_TOUCH) {
        int i;
        for (i = 0; i < fwCount; ++i) {
            if (fwEntry[i] == fwE) {
                return;
            }
        }

        if (i == fwCount) {
            fwClass[fwCount] = fwC;
            fwEntry[fwCount] = fwE;
            ++fwCount;
        }
    }
}

void CHitData::addAc(const uint8_t acC,const uint16_t acE) {
    if (acCount < MAX_TOUCH) {
        int i;
        for (i = 0; i < acCount; ++i) {
            if (acEntry[i] == acE) {
                return;
            }
        }

        if (i == acCount) {
            acClass[acCount] = acC;
            acEntry[acCount] = acE;
            ++acCount;
        }
    }
}

/////////////////////////////////////////////////////////////////////
// CMap()

CMap::CMap(const int sx, const int sy):
    m_len(sx),
    m_hei(sy)
{
}

CMap::~CMap()
{
}

void CMap::resize(const int len, const int hei)
{
    // Resize of the game map
    m_len = len;
    m_hei = hei;
    clear();
}

const Size CMap::size() const
{
    Size s;
    s.len = m_len;
    s.hei = m_hei;
    return s;
}

const Size CMap::size(const CFrame & frame)
{
    Size s;
    s.len = frame.m_nLen / GRID;
    s.hei = frame.m_nHei / GRID;
    return s;
}

const Size CMap::size(const int px, const int py)
{
    Size s;
    s.len = px / GRID;
    s.hei = py / GRID;
    return s;
}

const Pos CMap::toScreen(const int mapX, const int mapY)
{
    Pos p;
    p.x = mapX * GRID;
    p.y = mapY * GRID;
    return p;
}

const Pos CMap::toScreen(const Pos mapPos)
{
    return toScreen(mapPos.x, mapPos.y);
}

const Pos CMap::wScreen(const int screenX, const int screenY) const
{
    Pos p;
    p.x = screenX;
    p.y = screenY;

    // normalize the coordonates
    if (p.x < 0) {
        p.x += (m_len * GRID);
    }

    if (p.y < 0) {
        p.y += (m_hei * GRID);
    }

    p.x %= (m_len * GRID);
    p.y %= (m_hei * GRID);

   return p;
}

const Pos CMap::wScreen(const Pos screenPos) const
{
    return wScreen(screenPos.x, screenPos.y);
}

void CMap::clear()
{
    // Clear the map
    m_map.clear();
}

//////////////////////////////////////////////////////

CMapEntry::CMapEntry()
{
    memset(this, 0,sizeof(CMapEntry));
}

CMapEntry::~CMapEntry()
{

}

void CMapEntry::mapFw(uint8_t fwClass, uint16_t fwEntry)
{
    if (m_fwCount < MAX_FW) {
        m_nFwClass[m_fwCount] = fwClass;
        m_nFwEntry[m_fwCount] = fwEntry;
        ++m_fwCount;
    }
}

void CMapEntry::unMapFw(uint16_t fwEntry)
{
    int v = m_fwCount;
    for (int i=0, j=0; i < v; ++i) {
        if (m_nFwEntry[i] == fwEntry) {
            m_nFwClass[i] = 0;
            m_nFwEntry[i] = 0;
            --m_fwCount;
        } else {
            if (i != j) {
                m_nFwClass[j] = m_nFwClass[i];
                m_nFwEntry[j] = m_nFwEntry[i];
            }
            ++j;
        }
    }
}

void CMapEntry::getFw(int index, uint8_t & fwClass, uint16_t & fwEntry)
{
    fwClass = m_nFwClass[index];
    fwEntry = m_nFwEntry[index];
}

void CMapEntry::mapAc(uint8_t acClass, uint16_t acEntry)
{
    // TODO: implement this fully w/multiple support
    m_nAcClass = acClass;
    m_nAcEntry = acEntry;
}

void CMapEntry::unMapAc(uint16_t acEntry)
{
    // TODO: implement this fully w/multiple support
    if (acEntry == m_nAcEntry) {
        m_nAcClass = 0;
        m_nAcEntry = 0;
    }
}
