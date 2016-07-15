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

#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H

class IFile;
#include <string>

class CGameEvents
{
public:
    CGameEvents();
    ~CGameEvents();

    void read(IFile &file);
    void write(IFile & file);

    static int getSize();
    const char * getEvent(int i);
    static const char* getName(int i);
    void setEvent(int i, const char *s);
    const char *operator[] (int i);
    void forget();

    enum {
        EG_INIT_GAME,       // 0
        EG_PREPARE_LEVEL,   // 1
        EG_COMPLETE_LEVEL,  // 2
        EG_DEATH,           // 3
        EG_GOAL_COLLECTED,  // 4
        EG_GOAL_KILLED,     // 5
        EG_PICKUP,          // 6
        EG_LOADGAME,        // 7
        EG_SAVEGAME,        // 8
        EG_RESTART_LEVEL,   // 9
        EG_NOTIFYCLOSURE    // 10
    };

protected:
    std::string *m_events;
    static std::string m_eventList[];

};

#endif // GAMEEVENTS_H
