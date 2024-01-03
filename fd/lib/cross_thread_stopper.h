#pragma once

#include "ievent_source.h"

namespace libeventloop {

class IStoppable;

class CrossThreadStopper : public IEventSource {
public:
    CrossThreadStopper(IStoppable& s);
    ~CrossThreadStopper();

    int init();

    int stop();

    int onFD(const epoll_event& fd) final;

    std::vector<epoll_data_t> getFDs() final;

private:
    int m_event;
    IStoppable& m_stoppable;
};

} // namespace libeventloop