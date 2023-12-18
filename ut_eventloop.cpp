#include <iostream>
#include <vector>

#include "eventloop.h"

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

void test_all()
{
    test_eventloop_eventhandler();
}

}
