#include "microtime.h"

#include <chrono>

uint64_t microtime()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000;
}
