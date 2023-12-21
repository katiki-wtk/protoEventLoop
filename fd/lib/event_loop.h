#pragma once

#include "istoppable.h"
#include <unordered_map>
#include <vector>
#include <sys/epoll.h>
#include <functional>

namespace libeventloop {
class IEventSource;

using IEventSourceRefVector = std::vector<std::reference_wrapper<IEventSource>>;

/// Repeatedly notifies IEventSources when their FDs are ready.
class EventLoop : public IStoppable {
public:
    EventLoop();

    int init();

    /// Adds an event source.
    int addEventSource(IEventSource& s);
    int addEventSources(IEventSourceRefVector s);
    int removeEventSource(IEventSource& s);

    /// Loops handling events, until stop flag is set.
    int run();

    /// Runs one cycle of the event loop.
    /** If block is false, returns immediately. Otherwise blocks until something happens.
     */
    int runOnce(bool block = true);

    /// Sets stop flag. Call from an event-handler callback.
    void stop() final;

private:
    int m_EpollFd {-1};

    // Map from FDs to IEventSources.
    std::unordered_map<int, IEventSource*> m_fdMap;

    // Set this from an event handler to exit the loop.
    bool m_stop;

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&) = delete;
    EventLoop& operator=(EventLoop&&) = delete;
};
} // namespace libeventloop
