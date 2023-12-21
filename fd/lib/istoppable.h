#pragma once

namespace libeventloop {
class IStoppable {
public:
    virtual ~IStoppable() = default;

    virtual void stop() = 0;
};
} // namespace libeventloop