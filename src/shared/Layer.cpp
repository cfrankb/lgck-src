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

#include "stdafx.h"
#include "Layer.h"
#include "Level.h"
#include "LevelEntry.h"
#include <cstdio>
#include <zlib.h>
#include "IFile.h"
#include "LuaVM.h"
#include "helper.h"
#include "Game.h"
#include "Actor.h"

CLayer::CLayer(const char* name, int type, int h, int v)
{
    m_name = name;
    m_type = type;
    m_hSpeed = h;
    m_vSpeed = v;    
    m_max = GROWBY;
    m_arrEntries = new CLevelEntry[m_max];
    m_size = 0;
    m_mx = 0;
    m_my = 0;
}

CLayer::CLayer(int type)
{
    m_type = type;
    m_hSpeed = m_vSpeed = SPEED_WHOLE; 
    m_max = GROWBY;
    m_arrEntries = new CLevelEntry [m_max];
    m_size = 0;
}

CLayer::~CLayer()
{
    if (m_arrEntries) {
        delete [] m_arrEntries;
        m_arrEntries = nullptr;
    }
}

/////////////////////////////////////////////////////////////////////
// layer

int CLayer::add (CLevelEntry &entry)
{
    if (m_size == m_max) {
        m_max += GROWBY;
        CLevelEntry *t = new CLevelEntry[m_max];
        for (int i=0; i < m_size; ++i) {
            t[i] = m_arrEntries[i];
        }
        delete [] m_arrEntries;
        m_arrEntries = t;
    }
    m_arrEntries[m_size] = entry;
    return m_size++;
}

CLevelEntry & CLayer::operator [] (int n) const
{
    return (CLevelEntry &) m_arrEntries[n];
}

CLevelEntry & CLayer::atIndex(int n) const
{
    return (CLevelEntry &) m_arrEntries[n];
}

void CLayer::removeAt(int n)
{
    for (int i = n; i < m_size - 1; ++i) {
        m_arrEntries[i] = m_arrEntries[i + 1];
    }
    --m_size;
}

void CLayer::insertAt(int n, CLevelEntry & entry)
{
    CLevelEntry t = entry;
    add(t);
    t = entry;  // added for safety 09/12/22 !!! DO NOT REMOVE !!!
    for (int i = m_size - 1; i > n; --i) {
        m_arrEntries [i] = m_arrEntries [i - 1];
    }
    m_arrEntries[n] = t;
}

int CLayer::findProto (int nProto, int nStartAt)
{
    for (int i = nStartAt; i<m_size; ++i) {
        if (m_arrEntries[i].m_nProto == nProto)
            return i;
    }
    return - 1;
}

void CLayer::killProto(int nProto)
{
    // TODO: check if selection is affected
    m_selection.clear();
    for (int i = 0; i < m_size; ++i) {
        CLevelEntry & entry = m_arrEntries[i];
        if (entry.m_nProto == nProto) {
            removeAt(i);
            --i;
        } else {
            if (entry.m_nProto > nProto) {
                --entry.m_nProto;
            }
        }
    }
}

void CLayer::killFrameSet (int nFrameSet)
{
    // TODO: check if selection is affected
    m_selection.clear();
    for (int i = 0; i < m_size; ++i) {
        CLevelEntry & entry = m_arrEntries[i];
        if (entry.m_nFrameSet == nFrameSet) {
            removeAt(i);
            --i;
        } else {
            if (entry.m_nFrameSet > nFrameSet) {
                --entry.m_nFrameSet;
            }
        }
    }
}

CLayer & CLayer::operator=(CLayer & s)
{
    if (m_arrEntries) {
        delete [] m_arrEntries;
        m_arrEntries = nullptr;
    }
    m_selection.clear();
    m_size = s.getSize();
    m_max = m_size + GROWBY;
    m_arrEntries = new CLevelEntry[m_max];
    for (int i=0; i < m_size; ++i) {
        m_arrEntries[i] = s[i];
    }
    // copy the layer params
    int h;
    int v;
    m_name = s.getName();
    m_type = s.getType();
    s.getSpeed(h, v);
    setSpeed(h, v);
    // reset ptr
    m_mx = 0;
    m_my = 0;
    return *this;
}

bool CLayer::read(IFile & file, bool compr)
{
    int ver = LAYER_VER;
    int size = 0;
    int entrySize = 0;
    LONGUINT nTotalSize = 0;
    LONGUINT nCompressSize = 0;
    forget();
    file.read (&ver, sizeof (ver));
    file.read (&m_size, sizeof (size));
    m_max = m_size;
    file >> m_name;
    file >> m_type;
    if (ver > 1) {
        file >> m_hSpeed;
        file >> m_vSpeed;
    } else {
        int temp;
        file >> temp;
        m_hSpeed = m_vSpeed = SPEED_WHOLE;
    }
    file.read (&entrySize, sizeof(entrySize));
    file.read (&nTotalSize, 4);
    file.read (&nCompressSize,4);
    if (m_arrEntries) {
        delete [] m_arrEntries;
    }
    m_arrEntries = new CLevelEntry [ m_max ] ;
    if (compr) {
        uint8_t *pCompressData = new uint8_t [ nCompressSize ];
        if (nTotalSize != (m_size * sizeof(CLevelEntry))) {
            CLuaVM::debugv("CLayer() : total uncompressed size doesn't match array size\n");
            return false;
        }
        file.read(pCompressData, nCompressSize);
        int err = uncompress((uint8_t*)m_arrEntries,
                             &nTotalSize,
                             pCompressData,
                             nCompressSize);
        if (err) {
            CLuaVM::debugv("CLayer::Read err=%d\n", err);
            return false;
        }
        delete [] pCompressData;
    } else {
        file.read(m_arrEntries, nTotalSize);
    }
    if (ver < 3) {
        for (int i=0; i< m_size; ++i) {
            m_arrEntries[i].m_string = 0;
        }
    }
    if (ver > 3) {
        file.read(&m_mx, sizeof(m_mx));
        file.read(&m_my, sizeof(m_my));
    } else {
        m_mx = 0;
        m_my = 0;
    }
    return true;
}

bool CLayer::write(IFile &file, bool compr)
{
    int version = LAYER_VER;
    LONGUINT nTotalSize = 0;
    LONGUINT nCompressSize = 0;
    uint8_t *pCompressData = nullptr;
    file.write(&version,4);           // layer version
    file.write(&m_size,4);
    file << m_name;
    file << m_type;
    file << m_hSpeed;
    file << m_vSpeed;
    file << (int) sizeof (CLevelEntry);
    nTotalSize = m_size * sizeof (CLevelEntry);
    if (compr) {
        int err = compressData((uint8_t *)m_arrEntries, (LONGUINT)nTotalSize, &pCompressData, nCompressSize);
        if (err != Z_OK) {
            CLuaVM::debugv("CLayer::Write error: %d", err);
        }
        file << (int) nTotalSize;
        file << (int) nCompressSize;
        file.write(pCompressData, nCompressSize);
        delete [] pCompressData;
    } else {
        file << (int) nTotalSize;
        file << (int) nTotalSize;
        file.write(m_arrEntries, nTotalSize);
    }
    file.write(&m_mx, sizeof(m_mx));
    file.write(&m_my, sizeof(m_my));
    return true;
}

void CLayer::setName(const char * name)
{
    m_name = name;
}

const char * CLayer::getName()
{
    return m_name.c_str();
}

int CLayer::getType() {
    return m_type;
}

void CLayer::setType(int type) {
     m_type = type;
}

void CLayer::getSpeed(int &hSpeed, int &vSpeed){
    hSpeed = m_hSpeed;
    vSpeed = m_vSpeed;
}

void CLayer::setSpeed(const int hSpeed, const int vSpeed){
    m_hSpeed = hSpeed;
    m_vSpeed = vSpeed;
}

CLevelEntry & CLayer::getSelection(int i)
{
    int index = m_selection.getIndex(i);
    return m_arrEntries[index];
}

int CLayer::getSelectionIndex(int i)
{
    return m_selection.getIndex(i);
}

void CLayer::setSelectionIndex(int i, int index)
{
    m_selection.setIndex(i, index);
}

void CLayer::removeSelectedSprites()
{
    for (; m_selection.getSize(); ) {
        // remove LayerEntry
        int j = m_selection.getIndex(0);
        removeAt(j);
        // remove selection
        m_selection.removeAt(0);
        for (int k=0; k < m_selection.getSize(); ++k ) {
            int i = m_selection.getIndex(k);
            if (i > j) {
                m_selection.setIndex(k, --i);
            }
        }
    }
}

void CLayer::unSelectedAt(int i)
{
    m_selection.removeAt(i);
}

bool CLayer::isSingleSelection()
{
    return m_selection.isSingle();
}

bool CLayer::isMultiSelection()
{
    return m_selection.isMulti();
}

void CLayer::clearSelection()
{
    m_selection.clear();
}

void CLayer::selectSingle(int i)
{
    m_selection.clear();
    if (i != -1) {
        m_selection.addEntry(m_arrEntries[i], i);
    }
}

void CLayer::select(int i)
{
    m_selection.addEntry(m_arrEntries[i], i);
}

void CLayer::select(CSelection & selection)
{
    m_selection += selection;
}

int CLayer::getSelectionSize()
{
    return m_selection.getSize();
}

bool CLayer::isInSelection(int index)
{
    for (int i=0; i < m_selection.getSize(); ++i) {
        if (index == m_selection.getIndex(i)) {
            return true;
        }
    }
    return false;
}

void CLayer::removeFromSelection(int index)
{
    for (int i=0; i < m_selection.getSize(); ++i) {
        if (index == m_selection.getIndex(i)) {
            m_selection.removeAt(i);
            --i;
        }
    }
}

void CLayer::setOffset(int mx, int my)
{
    m_mx = mx;
    m_my = my;
}

void CLayer::getOffset(int & mx, int & my)
{
    mx = m_mx;
    my = m_my;
}

int CLayer::countSpriteOfClass(CGame & game, int spriteClass)
{
    int count = 0;
    for (int i=0; i < m_size; ++i) {
        CProto & proto = game.protos().getProto(m_arrEntries[i]);
        if (proto.m_nClass == spriteClass) {
            ++ count;
        }
    }
    return count;
}

int CLayer::countGoals()
{
    int count = 0;
    for (int i=0; i < m_size; ++i) {
        CLevelEntry & entry = m_arrEntries[i];
        CActor actor = CActor(entry);
        if (actor.isGoal()) {
            ++ count;
        }
    }
    return count;
}

CSelection & CLayer::selection()
{
    return m_selection;
}

void CLayer::resyncSelection()
{
    for (int i=0; i < m_selection.getSize(); ++i) {
        int j = m_selection.getIndex(i);
        m_selection.resync(m_arrEntries[j], i);
    }
}
