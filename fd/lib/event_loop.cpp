
#include "event_loop.h"
#include "ievent_source.h"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <set>
#include <sys/epoll.h>
#include <sys/time.h>
#include <vector>
#include <iostream>

namespace libeventloop {
EventLoop::EventLoop() : m_stop(false)
{}

int EventLoop::init() {
    m_EpollFd = epoll_create1(EPOLL_CLOEXEC);
    if (m_EpollFd < 0) {
        return -errno;
    }
    return 0;
}

void EventLoop::stop()
{
    m_stop = true;
}

int EventLoop::addEventSource(IEventSource& source)
{
    std::vector<epoll_data_t> items = source.getFDs();

    for (const auto& item : items) {
        if (item.fd != -1) {
            if (m_fdMap.find(item.fd) != m_fdMap.end()) {
                std::cerr << "duplicate fd " << item.fd << std::endl;
                return -1;
            }
            m_fdMap.insert(std::make_pair(item.fd, &source));
        }
        else {
            std::cerr << "fd not set" << std::endl;
            return -1;
        }

        struct epoll_event e = {
            .events = EPOLLIN
        };
        e.data.fd = item.fd;

        if (epoll_ctl(m_EpollFd, EPOLL_CTL_ADD, item.fd, &e) < 0) {
            return -errno;
        }
    }
    return 0;
}

int EventLoop::addEventSources(IEventSourceRefVector sources)
{
    int res = 0;
    for (auto& source : sources) {
        if (0 != (res = addEventSource(source))) {
            std::cerr << "Failed addding event source with code : " << res << std::endl;
            break;
        }
    }
    return res;
}

int EventLoop::removeEventSource(IEventSource& source)
{
    std::vector<epoll_data_t> items = source.getFDs();

    for (const auto& item : items) {
        if (item.fd != -1) {
            m_fdMap.erase(item.fd);
            
            struct epoll_event e = {
                .events = EPOLLIN
            };
            e.data.fd = item.fd;

            if (epoll_ctl(m_EpollFd, EPOLL_CTL_DEL, item.fd, &e) < 0) {
                return -errno;
            }
        }
        else {
            std::cerr << "fd not set" << std::endl;
            return -1;
        }
    }
    return 0;
}

int EventLoop::run()
{
    int res = 0;
    m_stop = false;
    while (!m_stop) {
        if (0 != (res = runOnce(true))) {
            std::cerr << "Failed polling with code : " << res << std::endl;
            break;
        }
    }
    return res;
}

int EventLoop::runOnce(bool block)
{
    struct epoll_event epoll_buf[32];
    struct epoll_event *e;

    int r = epoll_wait(m_EpollFd, epoll_buf, 32, (block ? -1 : 0));
    if (r < 0 && errno != EINTR) {
        return -errno;
    }

    if (r > 0) {
        for (e = epoll_buf; e < &epoll_buf[r]; ++e) {
            
            auto it = m_fdMap.find(e->data.fd);
            if (it == std::end(m_fdMap))
            {
                return -1;
            }
            // TODO handle return value
            it->second->onFD(*e);
        }
    }

    return 0;
}
} // namespace libeventloop
