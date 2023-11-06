#pragma once

#include "ievent_source.h"
#include <unordered_map>
#include <vector>

namespace libeventloop {
/// Base class for event sources that aggregate other event sources.
class FileEventSource : public IEventSource {
public:
    FileEventSource() {}

private:
    // Called by EventLoop when an item is signalled.
    int onFD(const epoll_event& fd) override {return 0;}

    // Returns the items managed by this event source.
    std::vector<epoll_data_t> getFDs() override {return {};}

    FileEventSource(const FileEventSource&) = delete;
    FileEventSource& operator=(const FileEventSource&) = delete;
    FileEventSource(FileEventSource&&) = delete;
    FileEventSource& operator=(FileEventSource&&) = delete;
};
} // namespace libeventloop