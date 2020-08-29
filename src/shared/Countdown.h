#ifndef _COUNTDOWN__H
#define _COUNTDOWN__H

#include <unordered_map>
#include <string>
#include "ISerial.h"

class Countdown: public ISerial
{
public:
    Countdown();
    ~Countdown();

    typedef struct {
        std::string counter;
        std::string payload;
    } CounterdownEntry;

    virtual bool read(IFile & file);
    virtual bool write(IFile & file);
protected:
    std::unordered_map <std::string, CounterdownEntry> m_countdown;
};
#endif
