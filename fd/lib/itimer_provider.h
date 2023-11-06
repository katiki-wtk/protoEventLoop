#pragma once

namespace libeventloop {
class ITimerProvider {
public:
    virtual ~ITimerProvider() = default;
};
} // namespace libeventloop