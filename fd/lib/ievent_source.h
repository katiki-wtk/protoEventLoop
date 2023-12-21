#pragma once

#include <vector>
#include <sys/epoll.h>

namespace libeventloop {
/// An IEventSource manages a set of FDs that can be polled with epoll.
class IEventSource {
public:
    virtual ~IEventSource() {}

    /// Called by EventLoop when an FD is signalled.
    virtual int onFD(const epoll_event& fd) = 0;

    /// Returns the FDs managed by this event source.
    virtual std::vector<epoll_data_t> getFDs() = 0;

protected:
    IEventSource() {}

private:
    IEventSource(const IEventSource&) = delete;
    IEventSource& operator=(const IEventSource&) = delete;
};
} // namespace libeventloop
