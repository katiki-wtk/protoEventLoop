#include "timer_base.h"

#include <iostream>
#include <cerrno>
#include <cstring>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace libeventloop {
TimerBase::TimerBase() : m_fd(-1)
{}

int TimerBase::init(int clock)
{
    if (m_fd == -1) {
        // Create timer.
        m_fd = ::timerfd_create(clock, TFD_NONBLOCK);
        if (m_fd < 0) {
            std::cerr << "timerfd_create failed with : " << std::strerror(errno) << std::endl;
            return -1;
        }
    }
    return 0;
}

TimerBase::~TimerBase()
{
    if (m_fd != -1) {
        (void)::close(m_fd);
    }
}

int TimerBase::stop()
{
    return set(0, 0);
}

std::vector<epoll_data_t> TimerBase::getFDs()
{
    return {{.fd = m_fd}};
}

void TimerBase::addClient(TimerCallback client)
{
    m_clients.push_back(client);
}

int TimerBase::onFD(const epoll_event& info)
{
    if (info.data.fd != m_fd) {
        std::cerr << "Unexpected fd :" << info.data.fd << "expeted :" << m_fd << std::endl;
        return -1;
    }

    if (info.events & EPOLLIN) {
        uint64_t expiries = 0;
        if (::read(m_fd, &expiries, sizeof(expiries)) == -1) {
            if (errno != EAGAIN && errno != ECANCELED) {
                std::cerr << "Error reading timer descriptor" << std::endl;
                return -1;
            }
        }
        return notify(expiries);
    }
    else if (info.events & EPOLLERR) {
        /// TODO: Remove timer event source from event loop?
        std::cerr << "Error polling timer" << std::endl;
        return -1;
    }

    // Any other poll events unhandled
    return -1;
}

int TimerBase::set(unsigned long delayMillis, unsigned long periodMillis, int flags)
{
    // Set timer parameters.
    struct itimerspec spec;
    millisToTimespec(delayMillis, spec.it_value);
    millisToTimespec(periodMillis, spec.it_interval);

    int r = ::timerfd_settime(m_fd, flags, &spec, nullptr);
    if (r < 0) {
        std::cerr << "timerfd_settime failed with : " << std::strerror(errno) << std::endl;
        return -1;
    }
    else {
        return 0;
    }
}

int TimerBase::notify(uint64_t expiries)
{
    int res = 0;
    for (auto client : m_clients) {
        if (0 != (res = client(this, expiries))) {
            std::cerr << "Failed calling timer callback with code : " << res << std::endl;
            break;
        }
    }
    return 0;
}

void TimerBase::millisToTimespec(unsigned long millis, struct timespec& ts)
{
    unsigned long secs = millis / 1000;
    unsigned long ms = millis % 1000;
    ts.tv_sec = secs;
    ts.tv_nsec = ms * 1000000;
}
} // namespace libeventloop
