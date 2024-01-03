#include "ut_fdtimers.h"

#include <QDebug>

#include <iostream>

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
    qDebug() << __FUNCTION__  << "Test Observer";
    EventLoop evLoop;

    libeventloop::TimerProvider provider;

    libeventloop::OneShotTimerPost timer(evLoop);
    timer.init();
    provider.addTimer(timer);

    bool stop = false;

    timer.addClient(
        [&](libeventloop::IEventSource* source, uint64_t expiries){
            qDebug() << __FUNCTION__ << "Timer tick !";
            return 0;
        }
    );
    timer.addClient(
        [&](libeventloop::IEventSource* source, uint64_t expiries){
            qDebug() << __FUNCTION__ << "Timer tick ! Echo !";
            stop = true;
            return 0;
        }
    );
    timer.start(3000);

    while (!stop);

    qDebug() << __FUNCTION__ << " - Done !";
}

/*
struct TimerSlots {
    void slot1(uint64_t expiries) {
        qDebug() << __FUNCTION__ << "Timer tick !";
    }

    void slot2(uint64_t expiries)
    {
        qDebug() << __FUNCTION__ << "Timer tick ! Echo !";
    }
};
*/

void test_signal_slot_function() {
    
    qDebug() << __FUNCTION__  << "Test Signal Slots";
    EventLoop evLoop;

    libeventloop::TimerProvider provider;

    libeventloop::OneShotTimerSignal timer;

    timer.init();
    provider.addTimer(timer);

    /*
    TimerSlots slots;

    timer.addClient<&TimerSlots::slot2>(&slots);
    */

    bool stop = false;
    
    timer.addClient(
        [&](uint64_t expiries){
            qDebug() << __FUNCTION__ << "Timer tick !";
            stop = true;
        }
    );

    timer.start(3000);

    while (!stop);

    qDebug() << __FUNCTION__ << " - Done !";
}


void test_all()
{
    test_simple_function();
    test_signal_slot_function();
}

}
