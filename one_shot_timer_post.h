#pragma once

#include "fd/lib/one_shot_timer.h"
#include "eventloop.h"

namespace libeventloop {
class OneShotTimerPost : public OneShotTimer {
public:
    OneShotTimerPost(EventLoop& ev);

    int notify(uint64_t expiries) override;
private :
    EventLoop& m_loop;
};
} // namespace libeventloop
