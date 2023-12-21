#include "lib/event_loop.h"
#include "lib/one_shot_timer.h"
#include <iostream>

int main(int argc, char *argv[])
{
    libeventloop::EventLoop loop;
    loop.init();
    libeventloop::OneShotTimer timer;
    timer.init();
    timer.addClient(
        [&](libeventloop::IEventSource* source, uint64_t expiries){
            loop.stop();
            std::cout << "Timer tick !" << std::endl;
            return 0;
        }
    );
    loop.addEventSource(timer);
    timer.start(3000);
    loop.run();
    std::cout << "Loop done !" << std::endl;
    return 0;
}