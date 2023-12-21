#ifndef UT_EVENTLOOP_H
#define UT_EVENTLOOP_H

/*!
 * Basic unit-tests without testing framework yet...
 */
namespace ut_eventloop
{

void test_eventloop_eventhandler();
void test_send_synchronous();
void test_send_synchronous_with_signalslots();
void test_mock_incoming_mqtt_event();
void test_mock_msupasyncremoterequest();
void test_all();


}
#endif // UT_EVENTLOOP_H
