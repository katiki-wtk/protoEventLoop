#pragma once

#include "ievent_source.h"
#include <unordered_map>
#include <vector>

namespace libeventloop {
/// Base class for event sources that aggregate other event sources.
class GenericEventSource : public IEventSource {
public:
    GenericEventSource() {}

private:
    // Called by EventLoop when an item is signalled.
    int onFD(const epoll_event& fd) override {return 0;}

    // Returns the items managed by this event source.
    std::vector<epoll_data_t> getFDs() override {return {};}

    GenericEventSource(const GenericEventSource&) = delete;
    GenericEventSource& operator=(const GenericEventSource&) = delete;
    GenericEventSource(GenericEventSource&&) = delete;
    GenericEventSource& operator=(GenericEventSource&&) = delete;
};
} // namespace libeventloop