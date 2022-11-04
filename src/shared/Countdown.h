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
#ifndef _COUNTDOWN__H
#define _COUNTDOWN__H

#include <unordered_map>
#include <string>
#include "ISerial.h"
#include "IFile.h"

class CCountdownEntry{
public:

    void start() {
        m_running = true;
    }

    void stop() {
        m_running = false;
    }

    bool isRunning() {
        return m_running;
    }
    uint64_t nextSecond() {
        return m_nextSecond;
    }
    void setNextSecond(uint64_t nxSecond) {
        m_nextSecond = nxSecond;
    }

    bool read(IFile & file) {
        file >> m_payload;
        file.read(&m_nextSecond, sizeof(m_nextSecond));
        file.read(&m_running, 1);
        return true;
    }

    bool write(IFile & file) {
        file << m_payload;
        file.write(&m_nextSecond, sizeof(m_nextSecond));
        file.write(&m_running, 1);
        return true;
    }

    std::string m_payload;
    uint64_t m_nextSecond;
    bool m_running;

    friend class CCountdown;
};

class CCountdown: public ISerial
{
public:
    CCountdown();
    ~CCountdown();

    virtual bool read(IFile & file);
    virtual bool write(IFile & file);
    CCountdownEntry & add(const char *name, int timeInSeconds=-1, uint64_t nextSecond=0, const char *payload=nullptr);
    void remove(const char *name);
    CCountdownEntry & get(const char *name);
    void forget();
    void cycle();
    CCountdownEntry & operator [](const char *name);
    int count();
    void removeAll() { forget(); }
    void reset() { m_toClear = true; }

protected:
    std::unordered_map <std::string, CCountdownEntry> m_countdown;
    bool m_toClear;
};
#endif
