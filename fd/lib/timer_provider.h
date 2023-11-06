#pragma once

#include "composite_event_source.h"
#include <unordered_map>
#include <vector>
#include "itimer_provider.h"

namespace libeventloop {
/// Base class for event sources that aggregate other event sources.
class TimerProvider : public CompositeEventSource, public ITimerProvider {
public:
    TimerProvider() {}

private:
    // Called by EventLoop when an item is signalled.
    int onFD(const epoll_event& fd) override {return 0;}

    // Returns the items managed by this event source.
    std::vector<epoll_data_t> getFDs() override {return {};}

    TimerProvider(const TimerProvider&) = delete;
    TimerProvider& operator=(const TimerProvider&) = delete;
    TimerProvider(TimerProvider&&) = delete;
    TimerProvider& operator=(TimerProvider&&) = delete;
};
} // namespace libeventloop