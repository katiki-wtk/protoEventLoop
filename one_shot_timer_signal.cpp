#include "one_shot_timer_signal.h"
#include "fd/lib/one_shot_timer.h"

#include <iostream>

namespace libeventloop {
OneShotTimerSignal::OneShotTimerSignal() : OneShotTimer()
{}

int OneShotTimerSignal::notify(uint64_t expiries) {
    std::cout << "Callback" << std::endl;
    m_timerTick.notify(expiries);
    return 0;
}

} // namespace libeventloop

