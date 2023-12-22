#include "one_shot_timer_signal.h"

#include "fd/lib/one_shot_timer.h"

namespace libeventloop {
OneShotTimerSignal::OneShotTimerSignal() : OneShotTimer()
{}

void OneShotTimerSignal::addClient(Slot client) {
    // m_timerTick.connect(client);
}

int OneShotTimerSignal::notify(uint64_t expiries) {
    m_timerTick.notify(expiries);
    return 0;
}

} // namespace libeventloop

