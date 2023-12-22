
#include "timer_provider.h"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <set>
#include <sys/epoll.h>
#include <sys/time.h>
#include <vector>
#include <iostream>

namespace libeventloop {

int TimerProvider::init() {
    m_EpollFd = epoll_create1(EPOLL_CLOEXEC);
    if (m_EpollFd < 0) {
        return -errno;
    }
    return 0;
}

void TimerProvider::stop()
{
    m_stop = true;
}

int TimerProvider::addTimer(TimerBase& source)
{
    auto items = source.getFDs();

    int fd = items[0].fd;

    if (fd != -1) {
        if (m_timerMap.find(fd) != m_timerMap.end()) {
            std::cerr << "duplicate fd " << fd << std::endl;
            return -1;
        }
        m_timerMap.insert(std::make_pair(fd, &source));
    }
    else {
        std::cerr << "fd not set" << std::endl;
        return -1;
    }

    struct epoll_event e = {
        .events = EPOLLIN
    };
    e.data.fd = fd;

    if (epoll_ctl(m_EpollFd, EPOLL_CTL_ADD, fd, &e) < 0) {
        return -errno;
    }
    
    return 0;
}

int TimerProvider::removeTimer(TimerBase& source)
{
    auto items = source.getFDs();

    int fd = items[0].fd;

    std::

    if (fd != -1) {
        m_timerMap.erase(fd);
    }
    else {
        std::cerr << "fd not set" << std::endl;
        return -1;
    }

    struct epoll_event e = {
        .events = EPOLLIN
    };
    e.data.fd = fd;

    if (epoll_ctl(m_EpollFd, EPOLL_CTL_DEL, fd, &e) < 0) {
        return -errno;
    }
    
    return 0;
}

int TimerProvider::run()
{
    int res = init();
    if (res == -1) {
        std::cerr << "Failed init" << std::endl;
    }
    m_stop = false;
    while (!m_stop) {
        if (0 != (res = runOnce(true))) {
            std::cerr << "Failed polling with code : " << res << std::endl;
            break;
        }
    }
    return res;
}

int TimerProvider::runOnce(bool block)
{
    struct epoll_event epoll_buf[32];
    struct epoll_event *e;

    int r = epoll_wait(m_EpollFd, epoll_buf, 32, (block ? -1 : 0));
    if (r < 0 && errno != EINTR) {
        return -errno;
    }

    if (r > 0) {
        for (e = epoll_buf; e < &epoll_buf[r]; ++e) {
            
            auto it = m_timerMap.find(e->data.fd);
            if (it == std::end(m_timerMap))
            {
                return -1;
            }
            // TODO handle return value
            m_loop.post([&](){
                it->second->onFD(*e);
            });
        }
    }

    return 0;
}
} // namespace libeventloop
