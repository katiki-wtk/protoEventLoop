#pragma once

#include "fd/lib/ievent_source.h"
#include "fd/lib/istoppable.h"
#include "fd/lib/timer_base.h"
#include "fd/lib/cross_thread_stopper.h"
#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <thread>

namespace libeventloop {
class TimerProvider : public IStoppable{
public:
    TimerProvider();

    ~TimerProvider();

    int init();

    int addTimer(TimerBase& s);
    int removeTimer(TimerBase& s);

    void start() { m_task = std::thread(&TimerProvider::run, this);}

    int run();
    int runOnce(bool block = true);
    void stop() final;

private:
    int addEventSource(IEventSource& s);
    int removeEventSource(IEventSource& s);

    std::thread m_task;

    int m_EpollFd {-1};

    CrossThreadStopper m_stopper;

    std::unordered_map<int, IEventSource*> m_fdMap;

    std::mutex m_mutex;

    bool m_stop {false};

    TimerProvider(const TimerProvider&) = delete;
    TimerProvider& operator=(const TimerProvider&) = delete;
    TimerProvider(TimerProvider&&) = delete;
    TimerProvider& operator=(TimerProvider&&) = delete;
};
} // namespace libeventloop
