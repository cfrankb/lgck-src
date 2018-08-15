#include "stdafx.h"

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
        memcpy(tmp, m_entries, sizeof(CLevelEntry) * m_size);
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

    m_entries = new CLevelEntry[s.getSize()];
    for (int i=0; i < s.getSize(); ++i) {
        m_entries[i] = s[i];
    }
}

void CSelection::forget()
{
    clear();
}
