#include "one_shot_timer.h"

namespace libeventloop {
OneShotTimer::OneShotTimer() : TimerBase()
{}

int OneShotTimer::start(unsigned long delayMillis)
{
    return set(delayMillis, 0);
}

} // namespace libeventloop
