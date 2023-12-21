#pragma once

#include "timer_base.h"

namespace libeventloop {
/// A timer that goes off at regular intervals.
class PeriodicTimer : public TimerBase {
public:
    /// Creates timer. Remember to call init().
    PeriodicTimer();

    /// Starts timer with given period.
    int start(unsigned long millis);
};
} // namespace libeventloop
