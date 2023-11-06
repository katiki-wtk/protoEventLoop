#pragma once

#include "ievent_source.h"
#include <unordered_map>
#include <vector>

namespace libeventloop {
/// Base class for event sources that aggregate other event sources.
class CompositeEventSource : public IEventSource {
public:
    /// Adds a child, which is assumed to be owned by derived class.
    int addChild(IEventSource& child);
    /// Removes a child.
    int removeChild(IEventSource& source);

protected:
    // This is an ABC.
    CompositeEventSource();

private:
    std::vector<epoll_data_t> m_items;
    std::unordered_map<int, IEventSource*> m_fdMap;

    // Called by EventLoop when an item is signalled.
    int onFD(const epoll_event& fd) override;

    // Returns the items managed by this event source.
    std::vector<epoll_data_t> getFDs() override;

    CompositeEventSource(const CompositeEventSource&) = delete;
    CompositeEventSource& operator=(const CompositeEventSource&) = delete;
    CompositeEventSource(CompositeEventSource&&) = delete;
    CompositeEventSource& operator=(CompositeEventSource&&) = delete;
};
} // namespace libeventloop