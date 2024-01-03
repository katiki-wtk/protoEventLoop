#pragma once

#include "fd/lib/one_shot_timer.h"
#include "signalslot.h"
#include <type_traits>
#include <utility>

namespace libeventloop {
class OneShotTimerSignal : public OneShotTimer {
public:

    OneShotTimerSignal();

    template<class C>
    void addClient(C* object) const {
        m_timerTick.connect(object);
    }

    template <typename ... Args>
    void addClient(void(*func)(Args...)) const {
        m_timerTick.connect(func);
    }

    template<typename F>
    void addClient(F&& functor) const {
        m_timerTick.connect(functor);
    }

    int notify(uint64_t expiries) override;

private:
    test::Signal<uint64_t> m_timerTick;
};
} // namespace libeventloop
