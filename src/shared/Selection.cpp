/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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

#include "../shared/Selection.h"
#include "../shared/LevelEntry.h"
#include <cstring>

////////////////////////////////////////////////////////////////////
// CSelection

CSelection::CSelection()
{
    m_size = 0;
    m_max = GROWBY;
    m_entries = new CLevelEntry [m_max];
    m_index = new int [m_max];
}

CSelection::~CSelection()
{
    if (m_entries) {
        delete [] m_entries;
    }

    if (m_index) {
        delete [] m_index;
    }
}

CLevelEntry & CSelection::operator[] (int i)
{
    return m_entries[i];
}

void CSelection::addEntry(const CLevelEntry & entry, int index)
{
//    bool found = false;
    // check if this entry is already in the selection
    for (int i=0; i < m_size; ++i) {
        if (m_index[i] == index) {
            return;
        }
    }

    if (m_size == m_max) {
        m_max += GROWBY;
        CLevelEntry *tmp = new CLevelEntry[m_max];
        memcpy(static_cast<void*>(tmp), m_entries, sizeof(CLevelEntry) * m_size);
        delete [] m_entries;
        m_entries = tmp;

        int *tmpi = new int[m_max];
        memcpy(tmpi, m_index, sizeof(int) * m_size);
        delete [] m_index;
        m_index = tmpi;
    }

    m_entries[m_size] = entry;
    m_index [m_size] = index;
    ++m_size;
}

void CSelection::clear()
{
    m_size = 0;
}

bool CSelection::isSingle()
{
    return m_size == 1;
}

bool CSelection::isMulti()
{
    return m_size > 1;
}

int CSelection::getSize()
{
    return m_size;
}

int CSelection::getIndex(int i)
{
    return m_index[i];
}

void CSelection::setIndex(int i, int index)
{
    m_index[i] = index;
}

void CSelection::removeFromIndex(int index)
{
    // TODO: Doesn't work. Fix it

    for (int i=0; i < m_size; ++i) {
        if (m_index[i] == index) {
            // if this scriptENtry is present
            // remove it
            removeAt(i);
        } else {
            if (m_index[i] > index) {
                // decrement the index of all
                // entries above the one being
                // removed
                --m_index[i];
            }
        }
    }
}

void CSelection::removeAt(int i)
{
    // remove entry from the index
    for (int j = i; j < m_size-1; ++j ) {
        m_index [ j ] = m_index [ j + 1];
        m_entries[ j ] = m_entries [ j + 1];
    }

    --m_size;
}

void CSelection::removeProto(int proto)
{
    int j = 0;
    for (int i = 0; i < m_size; ++ i) {
        if (i != j && (m_entries[i].m_nProto != proto)) {
            m_entries[j] = m_entries[i];
            m_index[j] = m_index[i];
        } else if (m_entries[i].m_nProto == proto) {
            continue;
        }
        ++ j;
    }
    m_size = j;
}
 
void CSelection::applyDelta(int dx, int dy)
{
    for (int i=0; i < m_size; ++i) {
        m_entries[i].m_nX -= dx;
        m_entries[i].m_nY -= dy;
    }
}

void CSelection::operator =(CSelection & s)
{
    if (m_entries) {
        delete [] m_entries;
    }

    if (m_index) {
        delete [] m_index;
    }

    m_entries = new CLevelEntry[s.getSize()];
    m_index = new int[s.getSize()];
    m_size = s.getSize();
    m_max = m_size;
    for (int i=0; i < s.getSize(); ++i) {
        m_entries[i] = s[i];
        m_index[i] = s.getIndex(i);
    }
}

void CSelection::operator +=(CSelection & s)
{
    for (int i=0; i < s.getSize(); ++i) {
        addEntry(s[i], s.getIndex(i));
    }
}

void CSelection::forget()
{
    clear();
}

/* compare the bitwise mask of selectionItems */
TriggerMask CSelection::compareMask()
{
    TriggerMask mask = {0,0,1,1};
    CLevelEntry & entry0 = m_entries[0];
    for (int i=1; i < getSize(); ++i) {
        CLevelEntry & entry = m_entries[i];
        mask.actionMaskDiff |= (entry.m_nActionMask ^ entry0.m_nActionMask);
        mask.triggerKeyDiff |= (entry.m_nTriggerKey ^ entry0.m_nTriggerKey);
        mask.sameImage &= (entry.m_nFrameNo == entry0.m_nFrameNo)
                && (entry.m_nFrameSet == entry0.m_nFrameSet) ? 1 : 0;
        mask.sameProto &= (entry.m_nProto == entry0.m_nProto) ? 1 : 0;
    }
    return mask;
}

bool CSelection::operator == (CSelection & src)
{
    if (src.getSize() != getSize()) {
        return false;
    }
    for (int i=0; i < getSize(); ++i) {
        if (m_entries[i] != src[i]) {
            return false;
        }
    }
    return true;
}

bool CSelection::operator != (CSelection & src)
{
    return !(*this == src);
}

void CSelection::resync(const CLevelEntry & entry, int index)
{
    m_entries[index] = entry;
}

CLevelEntry &CSelection::cacheAtIndex(int index)
{
    return m_entries[index];
}
