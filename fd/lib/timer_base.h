#pragma once

#include "ievent_source.h"
#include <cstdint>
#include <sys/timerfd.h>
#include <vector>
#include <functional>

namespace libeventloop {

using TimerCallback = std::function<int(IEventSource*, uint64_t)>;

/// Base class for PeriodicTimer and OneShot. Handles all the IEventSource stuff, the FD, the client
/// list.
class TimerBase : public IEventSource {
public:
    virtual ~TimerBase();

    /// 2nd phase ctor.
    int init(int clock = CLOCK_MONOTONIC);

    int stop();

    /// Adds a client.
    void addClient(TimerCallback client);

protected:
    // This is an ABC.
    TimerBase();

    // Derived classes use this to set the timer params. DelayMillis is delay to first occurrence,
    // periodMillis is interval between repeats.
    int set(unsigned long delayMillis, unsigned long periodMillis, int flags = 0);

private:
    int m_fd;
    static void millisToTimespec(unsigned long millis, struct timespec& ts);

    // Clients.
    std::vector<TimerCallback> m_clients;
    int notify(uint64_t expiries);

    int onFD(const epoll_event& info) override;

    std::vector<epoll_data_t> getFDs() override;

    TimerBase(const TimerBase&) = delete;
    TimerBase& operator=(const TimerBase&) = delete;
    TimerBase(TimerBase&&) = delete;
    TimerBase& operator=(TimerBase&&) = delete;
};
} // namespace libeventloop
