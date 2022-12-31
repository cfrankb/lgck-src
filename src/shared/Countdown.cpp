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
#include "Countdown.h"
#include "IFile.h"
#include "Game.h"
#include <vector>
// #include "stdafx.h"
#include "microtime.h"

CCountdown::CCountdown()
{
    m_toClear = false;
}

CCountdown::~CCountdown()
{
}

bool CCountdown::read(IFile &file)
{
    forget();
    int32_t count = 0;
    file.read(&count, sizeof(int32_t));
    for (int i = 0; i < count; ++i)
    {
        std::string key;
        file >> key;
        CCountdownEntry entry;
        entry.read(file);
        m_countdown[key.c_str()] = entry;
    }
    return true;
}

bool CCountdown::write(IFile &file)
{
    int32_t count = m_countdown.size();
    file.write(&count, sizeof(int32_t));
    for (auto kv : m_countdown)
    {
        std::string key = kv.first;
        CCountdownEntry &entry = kv.second;
        file << kv.first;
        entry.write(file);
    }
    return true;
}

void CCountdown::forget()
{
    m_countdown.clear();
}

void CCountdown::cycle()
{
    if (m_toClear)
    {
        forget();
        m_toClear = false;
    }
    std::vector<std::string> d;
    uint64_t now = microtime();
    for (auto kv : m_countdown)
    {
        std::string key = kv.first;
        // CLuaVM::debugv("countdown key: %s", key.c_str());
        CCountdownEntry &entry = kv.second;
        if (entry.isRunning() && now > entry.nextSecond())
        {
            int &count = CGame::getGame().counter(key.c_str());
            if (count)
            {
                --count;
                entry.setNextSecond(now + 1000);
            }
            else
            {
                entry.stop();
                if (!entry.m_payload.empty())
                {
                    CGame::luaVM().exec(entry.m_payload.c_str());
                }
                // TODO: remove entry
                d.push_back(key.c_str());
                // CLuaVM::debugv("countdown key to be deleted: %s", key.c_str());
            }
        }
    }
    for (std::string i : d)
    {
        m_countdown.erase(i);
        CGame::getGame().m_counters.erase(i);
        CLuaVM::debugv("countdown deleted: %s", i.c_str());
    }
}

CCountdownEntry &CCountdown::operator[](const char *name)
{
    return get(name);
}

CCountdownEntry &CCountdown::get(const char *name)
{
    return m_countdown[name];
}

CCountdownEntry &CCountdown::add(const char *name, int timeInSeconds, uint64_t nextSecond, const char *payload)
{
    if (timeInSeconds != -1)
    {
        CGame::getGame().counter(name) = timeInSeconds;
    }
    if (nextSecond == 0)
    {
        uint64_t now = microtime();
        nextSecond = now + 1000;
    }
    m_countdown[name] = CCountdownEntry{
        payload ? payload : "",
        nextSecond,
        false};
    return m_countdown[name];
}

void CCountdown::remove(const char *name)
{
    m_countdown.erase(name);
}

int CCountdown::count()
{
    return m_countdown.size();
}
