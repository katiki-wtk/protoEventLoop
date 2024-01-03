#include "cross_thread_stopper.h"
#include "istoppable.h"
#include <cstring>
#include <iostream>
#include <sys/eventfd.h>
#include <unistd.h>

namespace libeventloop {

CrossThreadStopper::CrossThreadStopper(IStoppable& s) : m_event(-1), m_stoppable(s)
{}

CrossThreadStopper::~CrossThreadStopper()
{
    if (m_event != -1) {
        (void)::close(m_event);
    }
}

int CrossThreadStopper::init()
{
    m_event = ::eventfd(0, EFD_SEMAPHORE);
    if (m_event < 0) {
        std::cerr << "eventfd() failed with : " << std::strerror(errno) << std::endl;
        return errno;
    }
    else {
        return 0;
    }
}

int CrossThreadStopper::stop()
{
    std::cout << "Resquesting stop" << std::endl;
    uint64_t word{1};
    ssize_t r = ::write(m_event, &word, sizeof(word));
    if (r == -1) {
        std::cerr << "write() failed with : " << std::strerror(errno) << std::endl;
        return errno;
    }
    else {
        return 0;
    }
}

std::vector<epoll_data_t>  CrossThreadStopper::getFDs()
{
    return {{.fd = m_event}};
}

int CrossThreadStopper::onFD(const epoll_event& fd)
{
    // TODO may need to read event on sem
    m_stoppable.stop();
    return 0;
}

} // namespace libeventloop