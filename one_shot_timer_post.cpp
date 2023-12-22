#include "one_shot_timer_post.h"
#include "eventloop.h"
#include <iostream>


namespace libeventloop {
OneShotTimerPost::OneShotTimerPost(EventLoop& ev) : m_loop(ev), OneShotTimer()
{}


int OneShotTimerPost::notify(uint64_t expiries) {
    m_loop.post([this, &expiries]() {
        int res = 0;
        for (auto client : m_clients) {
            if (0 != (res = client(this, expiries))) {
                std::cerr << "Failed calling timer callback with code : " << res << std::endl;
                break;
            }
        }
    });

    return 0;
}

} // namespace libeventloop

