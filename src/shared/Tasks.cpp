/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2012  Francois Blanchette

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
#include "Tasks.h"
#include <cstring>
#include "IFile.h"

/////////////////////////////////////////////////////////////////////
// CTask

CTask::CTask()
{
    m_ticks = 0;
    m_task = 0 ;
    m_script = "";
}

void CTask::read(IFile & file, int version)
{
    Q_UNUSED(version);
    file.read(&m_ticks, sizeof(m_ticks));
    file.read(&m_task, sizeof(m_task));
    file >> m_script;
    m_actor.read(file,0);
}

void CTask::write(IFile &file)
{
    file.write(&m_ticks, sizeof(m_ticks));
    file.write(&m_task, sizeof(m_task));
    file << m_script;
    m_actor.write(file);
}

/////////////////////////////////////////////////////////////////////
// CTasks

CTasks::CTasks()
{
    m_size = 0;
    m_max = GROWBY;
    m_tasks = new CTask* [m_max];
}

CTasks::~CTasks()
{
    forget();
    if (m_tasks) {
        delete [] m_tasks;
    }
}

void CTasks::add(CTask *task)
{
    if (m_size == m_max) {
        m_max += GROWBY;
        CTask **t = new CTask * [m_max];
        for (int i=0; i< m_size; ++i) {
            t[i] = m_tasks[i];
        }
        delete [] m_tasks;
        m_tasks = t;
    }

    m_tasks[m_size] = task;
    ++m_size;
}

void CTasks::removeAt(int i)
{
    delete m_tasks[i];
  //  qDebug("delete [%d] size=%d", i, m_size);
    for (int j=i; j < m_size -1; ++j) {
    //    qDebug("m_tasks[%d] = m_tasks[%d]", j , j +1);
        m_tasks[j] = m_tasks[j + 1];
    }
    --m_size;
}

CTask * CTasks::operator [] (int i)
{
    return m_tasks[i];
}

int CTasks::getSize()
{
    return m_size;
}

void CTasks::forget()
{
    for (int i=0; i< m_size; ++i) {
        delete m_tasks[i];
        m_tasks[i] = NULL;
    }
    m_size = 0;
}

void CTasks::createTask(CActor & actor)
{
    CTask * task = new CTask;
    task->m_actor = actor;
    task->m_task = CTask::TASK_SPAWN;
    add(task);
}

void CTasks::read(IFile &file, int ver)
{
    Q_UNUSED(ver);
    int version = 0;
    file.read(&version, sizeof(version));
    ASSERT(version==VERSION);
    int size = 0;
    file.read(&size, sizeof(m_size));
    forget();
    for (int i=0; i< size; ++i) {
        CTask *task = new CTask;
        task->read(file, version);
        add(task);
    }
}

void CTasks::write(IFile &file)
{
    int version = VERSION;
    file.write(&version, sizeof(version));
    file.write(&m_size, sizeof(m_size));
    for (int i=0; i< m_size; ++i) {
        CTask * t = m_tasks[i];
        t->write(file);
    }
}
