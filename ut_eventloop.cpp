#include <iostream>
#include <string>
#include <vector>

#include "eventloop.h"
#include "signalslot.h"

using namespace test;

namespace ut_eventloop
{


struct DpuPacketEvent {
    char data;
};



// Event handler
std::function<void(const DpuPacketEvent&)> OnDpuEvent;


void emitDpuEvent(EventLoop& loop, const DpuPacketEvent& data)
{
    if (!OnDpuEvent) {
        std::cerr << __FUNCTION__ << " return..." << std::endl;
        return;
    }


    loop.post(std::bind(std::ref(OnDpuEvent), std::move(data)));
}



/*!
 * \brief test_eventloop_eventhandler
 */
void test_eventloop_eventhandler()
{
    EventLoop evLoop;

    OnDpuEvent = [] (auto ev) {
        std::cout << "EventHandler called: " << ev.data << std::endl;
    };

    emitDpuEvent(evLoop, DpuPacketEvent{'X'});
}

struct Observer1
{
    void observer1_slot(std::string ev)
    {
        std::cout << __FUNCTION__ << " - event: " << ev << std::endl;
    }
};

struct Observer2
{
    void observer2_slot(std::string ev)
    {
        std::cout << __FUNCTION__ << " - another event: " << ev << std::endl;
    }
};

/*!
 * \brief test_eventloop_observer_signalslot
 */
void test_eventloop_observer_signalslot()
{
    auto observer1 = std::make_unique<Observer1>();
    auto observer2 = std::make_unique<Observer2>();

    Signal<std::string> mySignal;

    EventLoop loop;

    mySignal.connect<&Observer1::observer1_slot>(observer1.get());
    mySignal.connect<&Observer2::observer2_slot>(observer2.get());


    loop.post([&mySignal](){
        mySignal.notify("Test !");
    });

}

void test_all()
{
    test_eventloop_eventhandler();
    test_eventloop_observer_signalslot();
}

}
