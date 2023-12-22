#pragma once

#include "eventloop.h"
#include "fd/lib/ievent_source.h"
#include "fd/lib/istoppable.h"
#include "fd/lib/timer_base.h"
#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <thread>

namespace libeventloop {
class TimerProvider : public IStoppable{
public:
    TimerProvider(EventLoop& loop) : m_loop{loop} {}

    ~TimerProvider() {
        stop();
        m_task.join();
    }

    int init();

    int addTimer(TimerBase& s);
    int removeTimer(TimerBase& s);

    int run();
    int runOnce(bool block = true);
    void stop() final;

private:
    EventLoop& m_loop;

    std::thread m_task{&TimerProvider::run, this} ;

    std::mutex m_mutex;

    int m_EpollFd {-1};

    std::unordered_map<int, IEventSource*> m_timerMap;

    std::atomic_bool m_stop;

    TimerProvider(const TimerProvider&) = delete;
    TimerProvider& operator=(const TimerProvider&) = delete;
    TimerProvider(TimerProvider&&) = delete;
    TimerProvider& operator=(TimerProvider&&) = delete;
};
} // namespace libeventloop
