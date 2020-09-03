#ifndef _COUNTDOWN__H
#define _COUNTDOWN__H

#include <unordered_map>
#include <string>
#include "ISerial.h"

class CCountdownEntry{
public:
    std::string m_counter;
    std::string m_payload;
    uint64_t m_nextSecond;
    int m_initialValue;
    bool m_running;
    void start() { m_running = true; }
    void stop() { m_running = false; }
};

class CCountdown: public ISerial
{
public:
    CCountdown();
    ~CCountdown();

    virtual bool read(IFile & file);
    virtual bool write(IFile & file);
    void add(int timeInSeconds, uint64_t nextSecond, const char *name=nullptr, const char *counter=nullptr);
    void forget();
    void cycle();
    void remove(const char *name);
    CCountdownEntry operator [] (const char *name);

protected:
    std::unordered_map <std::string, CCountdownEntry> m_countdown;
};
#endif
