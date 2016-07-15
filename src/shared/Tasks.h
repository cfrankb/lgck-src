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

#ifndef TASKS_H
#define TASKS_H

#include "Actor.h"
#include <string>

class IFile;

class CTask
{
public:
    CTask();

    unsigned long long m_ticks;
    int m_task;
    std::string m_script;
    CActor m_actor;
    void read(IFile &file, int version);
    void write(IFile & file);

    enum {
        TASK_SPAWN      = 0,
        TASK_REBIRTH    = 1,
        TASK_SCRIPT     = 2
    };

private:
};

class CTasks
{
public:
    CTasks();
    ~CTasks();

    void add(CTask *task);
    void removeAt(int i);
    CTask * operator [] (int i);
    int getSize();
    void forget();    
    void createTask(CActor & actor);
    void read(IFile & file, int version);
    void write(IFile & file);

protected:

    CTask ** m_tasks;
    int m_max;
    int m_size;

private:
    enum {
        GROWBY = 10,
        VERSION = 0x0001
    };
};

#endif // TASKS_H
