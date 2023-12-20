#include <QDebug>

#include <iostream>
#include <cstdlib>
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

    void observe1_slot2(int age, const std::string& name) {
        qDebug() << __FUNCTION__ << " - Name: " << QString::fromStdString(name) << ", age=" << age;
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


/*!
 * \brief test_send_synchronous
 */
void test_send_synchronous()
{
    auto lambda = [](int a, const std::string& name) -> double {
        qDebug() <<  __FUNCTION__ << " - Name: " << QString::fromStdString(name) << ", age=" << a;
        return static_cast<double>(a * name.length());

    };

    EventLoop evLoop;

    auto result = evLoop.send(lambda, 29, "Karim");

    qInfo() << " - result = " << result;
}

void test_send_synchronous_with_signalslots()
{
    qDebug() << __FUNCTION__;
    auto lambda = [](int a, const std::string& name) -> double {
        qDebug() <<  __FUNCTION__ << " @@@  - Name: " << QString::fromStdString(name) << ", age=" << a;
        return static_cast<double>(a * name.length());

    };

    Observer1 observer1;


    Signal<int, std::string> mySignal;
    mySignal.connect(lambda);
    mySignal.connect<&Observer1::observe1_slot2>(&observer1);

    EventLoop evLoop;

    evLoop.post([&mySignal]() {
        mySignal(29, "Karim");
    });

    qDebug() << __FUNCTION__ << " - Done !";
}


/*!
 * \brief test_incoming_mqtt_event
 */

void test_incoming_mqtt_event()
{
    struct Message
    {
        int id;
        std::string payload;
    };

    struct ExtcCommunicationMqtt
    {
        EventLoop& m_evLoop;

        void handlerOnMessageArrived(const std::string& data)
        {
            m_evLoop.post([this,&data]() {
                Message msg;
                fillData(msg, data);
                processMessage(std::move(msg));
            });
        }

        void fillData(Message& msg, const std::string data)
        {
            msg.id = std::stoi(data.substr(0, 2));
            msg.payload = data.substr(2);
        }

        void processMessage(Message&& msg)
        {
            qDebug() << __FUNCTION__ << " - Message ID=" << msg.id << ", Payload=" << QString::fromStdString(msg.payload);
        }

    };

    EventLoop evLoop;
    ExtcCommunicationMqtt extc {evLoop};

    extc.handlerOnMessageArrived("19PayloadData");


}

void test_all()
{
    test_eventloop_eventhandler();
    test_eventloop_observer_signalslot();
    test_send_synchronous();
    test_send_synchronous_with_signalslots();
    test_incoming_mqtt_event();
}

}
