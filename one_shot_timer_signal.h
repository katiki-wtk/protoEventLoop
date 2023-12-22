#pragma once

#include "fd/lib/one_shot_timer.h"
#include "signalslot.h"

namespace libeventloop {
class OneShotTimerSignal : public OneShotTimer {
public:
    using Slot = std::function<void(IEventSource*, uint64_t)>;

    OneShotTimerSignal();

    void addClient(Slot client);

    int notify(uint64_t expiries) override;

private:
    test::Signal<uint64_t> m_timerTick;
};
} // namespace libeventloop
