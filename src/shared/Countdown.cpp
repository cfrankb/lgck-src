#include "Countdown.h"
#include "IFile.h"
#include "helper.h"

CCountdown::CCountdown()
{

}

CCountdown::~CCountdown()
{

}

bool CCountdown::read(IFile & file)
{

}

bool CCountdown::write(IFile & file)
{

}

void CCountdown::forget()
{
    m_countdown.clear();
}

void CCountdown::cycle()
{

}

CCountdownEntry CCountdown::operator [] (const char *name)
{
    return m_countdown[name];
}

void CCountdown::add(int timeInSeconds, uint64_t nextSecond, const char *name, const char *counter)
{
    char * tmpCounter = counter ? nullptr : getUUID();
    char * tmpName = name ? nullptr : getUUID();
    CCountdownEntry c = {
        tmpCounter ? tmpCounter: counter,
        "",
        nextSecond,
        timeInSeconds,
        false
    };
    m_countdown[tmpName] = c;
    if (tmpName) {
        delete [] tmpName;
    }
    if (tmpCounter) {
        delete [] tmpCounter;
    }
}
