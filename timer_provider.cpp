
#include "timer_provider.h"
#include <cerrno>
#include <mutex>
#include <set>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/time.h>
#include <vector>
#include <iostream>
#include <unistd.h>

namespace libeventloop {

TimerProvider::TimerProvider() : m_stopper(*this){ init(); }

TimerProvider::~TimerProvider() {
    m_stopper.stop();
     
    if (m_EpollFd != -1) {
        (void)::close(m_EpollFd);
    }

    m_task.join();
}

int TimerProvider::init() {
    m_EpollFd = epoll_create1(EPOLL_CLOEXEC);
    if (m_EpollFd < 0) {
        std::cerr << "Failed to create epoll instance" << std::endl;
        return -errno;
    }

    m_stopper.init();

    std::cout << "Adding stopper" << std::endl;
    addEventSource(m_stopper);

    return 0;
}

void TimerProvider::stop()
{
    m_stop = true;
}

int TimerProvider::addTimer(TimerBase& source)
{
    return addEventSource(source);
}

int TimerProvider::removeTimer(TimerBase& source)
{
    return removeEventSource(source);
}


int TimerProvider::addEventSource(IEventSource& source) {
    auto items = source.getFDs();

    int fd = items[0].fd;

    if (fd != -1) {
        if (m_fdMap.find(fd) != m_fdMap.end()) {
            std::cerr << "duplicate fd " << fd << std::endl;
            return -1;
        }
        m_fdMap.insert(std::make_pair(fd, &source));
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
        std::cerr << "Failed to add fd " << fd << " to epoll instance " << m_EpollFd << " with errno :" << errno << std::endl;
        return -errno;
    }
    
    return 0;
}

int TimerProvider::removeEventSource(IEventSource& source) {
    auto items = source.getFDs();

    int fd = items[0].fd;

    if (fd != -1) {
        m_fdMap.erase(fd);
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
    int res = 0;
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

        std::cout << "Received size " << r << std::endl;

        for (e = epoll_buf; e < &epoll_buf[r]; ++e) {

            int fd {e->data.fd};

            auto it = m_fdMap.find(fd);
            if (it == std::end(m_fdMap))
            {
                return -1;
            } else {
                std::cout << "Event source is fd " << fd << std::endl;
            }

            // TODO handle return value
            it->second->onFD(*e); 
        }
    }

    return 0;
}
} // namespace libeventloop
