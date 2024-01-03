#include "ut_fdtimers.h"

#include <QDebug>

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include "eventloop.h"
#include "timer_provider.h"
#include "one_shot_timer_signal.h"
#include "one_shot_timer_post.h"

namespace ut_fdtimers
{


/*!
 * \brief test_eventloop_eventhandler
 */
void test_simple_function()
{
    std::cout << "Test" << std::endl;
    EventLoop evLoop;

    libeventloop::TimerProvider provider;

    libeventloop::OneShotTimerPost timer(evLoop);
    timer.init();
    provider.addTimer(timer);
    timer.addClient(
        [&](libeventloop::IEventSource* source, uint64_t expiries){
            qDebug() << __FUNCTION__ << "Timer tick !";
            return 0;
        }
    );
    timer.addClient(
        [&](libeventloop::IEventSource* source, uint64_t expiries){
            qDebug() << __FUNCTION__ << "Timer tick ! Echo !";
            return 0;
        }
    );
    timer.start(3000);

    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    qDebug() << __FUNCTION__ << " - Done !";
}


void test_signal_slot_function() {
    /*
    std::cout << "Test" << std::endl;
    EventLoop evLoop;

    libeventloop::TimerProvider provider(evLoop);

    libeventloop::OneShotTimerSignal timer;
    timer.init();
    provider.addTimer(timer);

    timer.start(3000);

    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    qDebug() << __FUNCTION__ << " - Done !";
    */
}


void test_all()
{
    test_simple_function();
    test_signal_slot_function();
}

}
