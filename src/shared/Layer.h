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

#ifndef LAYER_H
#define LAYER_H

#include "../shared/LevelEntry.h"
#include "../shared/Selection.h"
#include <string>
#include "../shared/ISurface.h"
class IFile;
class CGame;

class CLayer: public ISurface
{
public:
    CLayer(int type=LAYER_BK);
    CLayer(const char* name, int type=LAYER_BK, int h=0, int v=0);
    void setName(const char * name);
    const char * getName();
    ~CLayer();

    int add (CLevelEntry &entry);
    CLevelEntry & operator [] (int n) const;
    virtual CLevelEntry & atIndex (int n) const;
    void removeAt(int n);
    void insertAt (int n, CLevelEntry & entry);
    int findProto (int nProto, int nStartAt=0);
    void killProto(int nProto);
    void killFrameSet (int nFrameSet);
    CLayer & operator = (CLayer & source);
    virtual int getSize() const { return m_size; }
    void setSize( int size ) { m_size = size; }
    void forget() { m_size = 0; }
    bool read(IFile &, bool compr=true);
    bool write(IFile &, bool compr=true);
    int getType();
    void setType( int );
    void getSpeed(int &hSpeed, int &vSpeed);
    void setSpeed(const int hSpeed, const int vSpeed);

    CLevelEntry & getSelection(int i);
    int getSelectionIndex(int i);
    void setSelectionIndex(int i, int index);
    void removeSelectedSprites();
    bool isSingleSelection();
    bool isMultiSelection();
    void clearSelection();
    void selectSingle(int i);
    void select(int i);
    void select(CSelection & selection);
    int getSelectionSize();
    bool isInSelection(int index);
    void removeFromSelection(int index);
    void unSelectedAt(int i);
    CSelection & selection();

    void setOffset(int mx, int my);
    void getOffset(int & mx, int & my);
    int countSpriteOfClass(CGame &game, int spriteClass);
    int countGoals();

    enum {
        LAYER_BK,
        LAYER_MAIN,
        LAYER_FW
    } LAYER_TYPE;

    enum {
        SPEED_NOSCROLL,
        SPEED_WHOLE,
        SPEED_1_2,
        SPEED_1_4,
        SPEED_1_8,
        SPEED_1_16,
        SPEED_1_32,
        SPEED_1_64,
        SPEED_1_128,
        SPEED_CUSTOM = 0x10
    };

protected:

    CSelection m_selection;
    std::string m_name;
    int m_hSpeed;
    int m_vSpeed;
    int m_type;
    int m_size;
    int m_max;
    CLevelEntry *m_arrEntries;

    int m_mx;
    int m_my;

    enum {
        GROWBY = 32,
        LAYER_VER = 0x004
    };
};

#endif // LAYER_H
