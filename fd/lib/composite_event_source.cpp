#include "composite_event_source.h"
#include <algorithm>
#include <iostream>

namespace libeventloop {
CompositeEventSource::CompositeEventSource()
{}

int CompositeEventSource::addChild(IEventSource& source)
{
    auto items = source.getFDs();
    for (const auto& item : items) {
        if (item.fd >= 0) {
            if (m_fdMap.find(item.fd) != m_fdMap.end()) {
                std::cerr << "fd " << item.fd << " already in map" << std::endl;
                return -1;
            }
            m_fdMap.insert(std::make_pair(item.fd, &source));
        }
        else {
            std::cerr << "fd is not set" << std::endl;
            return -1;
        }
        m_items.push_back(item);
    }

    return 0;
}

int CompositeEventSource::removeChild(IEventSource& source)
{
    auto items = source.getFDs();

    for (const auto& item : items) {
        if (item.fd != -1) {
            m_fdMap.erase(item.fd);
            int fd = item.fd;
            auto newEnd = std::remove_if(
                std::begin(m_items), std::end(m_items), [&fd](auto const& item) {
                    return item.fd == fd;
                });
            m_items.erase(newEnd, std::end(m_items));
        }
        else {
            std::cerr << "fd is not set" << std::endl;
            return -1;
        }
    }
    
    return 0;
}

std::vector<epoll_data_t> CompositeEventSource::getFDs()
{
    return m_items;
}

int CompositeEventSource::onFD(const epoll_event & item)
{
    if (item.data.fd >= 0) {
        const auto it = m_fdMap.find(item.data.fd);
        if (it == m_fdMap.end()) {
            std::cerr << "fd " << item.data.fd << " not in map" << std::endl;
            return -1;
        }
        return it->second->onFD(item);
    }
    else {
        std::cerr << "fd is not set" << std::endl;
        return -1;
    }

    return 0;
}
} // namespace libeventloop
