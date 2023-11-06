#pragma once

#include "timer_base.h"

namespace libeventloop {
/// A timer that goes off once some number of milliseconds in the future.
class OneShotTimer : public TimerBase {
public:
    /// Remember to call init().
    OneShotTimer();

    /// Starts off the oneshot with the given duration.
    int start(unsigned long millis);
};
} // namespace libeventloop
