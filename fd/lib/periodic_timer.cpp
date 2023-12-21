#include "periodic_timer.h"

namespace libeventloop {
PeriodicTimer::PeriodicTimer()
{}

int PeriodicTimer::start(unsigned long periodMillis)
{
    return set(periodMillis, periodMillis);
}
} // namespace libeventloop
