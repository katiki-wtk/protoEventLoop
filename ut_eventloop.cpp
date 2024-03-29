#include <QDebug>

#include <iostream>
#include <exception>
#include <cstdlib>
#include <string>
#include <memory>
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

#include <chrono>


void test_lambda_shared_ptr()
{
    struct Data {
        std::string msg;
        Data(const std::string& m) : msg{m} {}
    };

    // shared_ptr
    {

        EventLoop evLoop;

        {
            auto data = std::make_shared<Data>("Test");

            evLoop.post([data]() {
                qDebug() << __FUNCTION__ << "Data = " << QString::fromStdString(data->msg);
            });
        }
    }

    // weak_ptr
    {
        EventLoop evLoop;

        {
            auto data = std::make_shared<Data>("Test");

            std::weak_ptr<Data> wp = data;

            evLoop.post([wp]() {
                auto sp = wp.lock();
                if (sp) {
                    qDebug() << __FUNCTION__ << "SP Data = " << QString::fromStdString(sp->msg);
                }
                else {
                    qDebug() << __FUNCTION__ << "Data NOT AVAILABLE ";
                    throw std::exception("error");
                }
            });

            //std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
}

/*!
 * \brief test_mock_incoming_mqtt_event
 */

void test_mock_incoming_mqtt_event()
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
            m_evLoop.post([this,data]() {
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

/*!
 * \brief test_mock_msupasyncremoterequest
 */

void test_mock_msupasyncremoterequest()
{
    struct FSM
    {
        void runTechnicalErrorRaisedRequest(const int_fast32_t p_X,
                                            const int_fast32_t p_Y,
                                            const int_fast32_t p_Z)
        {
            qDebug() << __FUNCTION__ << " x=" << p_X << "y=" << p_Y << ", z=" << p_Z;
        }

        void runMsupSwitchOffRequest(const int p_ReasonId)
        {
            qDebug() << __FUNCTION__ << " reasonId=" << p_ReasonId;
        }

        void runChangeLifecycleRequest(const int p_StateId)
        {
            qDebug() << __FUNCTION__ << " stateId=" << p_StateId;
        }


    };

    struct MsupAsyncRemoteRequest
    {
        EventLoop& m_evLoop;

        std::shared_ptr<FSM> m_fsm {nullptr};

        void pushTechnicalErrorRaisedRequest(const int_fast32_t p_X,
                                             const int_fast32_t p_Y,
                                             const int_fast32_t p_Z)
        {

            m_evLoop.post([=](){
                m_fsm->runTechnicalErrorRaisedRequest(p_X, p_Y, p_Z);
            }, true);

        }

        void pushSwitchOffRequest(const int p_SenderId, const int p_ReasonId)
        {
            m_evLoop.post([=](){
                m_fsm->runMsupSwitchOffRequest(p_ReasonId);
            });


        }

        void pushChangeLifecycleRequest(const int p_StateId)
        {
            m_evLoop.post([=](){
                m_fsm->runChangeLifecycleRequest(p_StateId);
            });

        }
    };

    EventLoop evLoop;
    auto fsm = std::make_shared<FSM>();
    MsupAsyncRemoteRequest msupAsyncRemoteReq{evLoop, fsm};

    for (int i=0 ; i<10 ; i++) {
        msupAsyncRemoteReq.pushTechnicalErrorRaisedRequest(i, i+1, i*10);
        msupAsyncRemoteReq.pushSwitchOffRequest(0x10, i);
        msupAsyncRemoteReq.pushChangeLifecycleRequest(i);
    }
}


void test_lambda_capture_move()
{
    struct Message {
        std::string msg;
        int val {0};
        Message(const std::string& m, int v)
            : msg(m), val(v){}
    };


    EventLoop loop;


    {
        auto msg = std::make_shared<Message>("test", 99);

        /*loop.post([msg = std::move(msg)]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cerr << "Message: " << msg.msg << ", val=" << msg.val << std::endl;

        });
*/
        loop.post([msg]() {
           std::this_thread::sleep_for(std::chrono::seconds(2));
            qDebug() << "Message: " << QString::fromStdString(msg->msg) << ", val=" << msg->val;

        });


    }


    /*
     * qDebug() << __FUNCTION__ << " - Go";

    struct Extc
    {
        EventLoop& loop;

        void onReceive(Message& msg)
        {
            loop.post([this, &msg]() {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                foo(msg);
            });
        }

        void onReceive(std::string& msg, int& val)
        {
            loop.post([this, &msg, &val]() {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                foo(msg, val);
            });
        }


        void foo(Message& msg)
        {
            //qDebug() <<  __FUNCTION__ << " -- Message:" << QString::fromStdString(msg.msg) << ", val=" << msg.val;
            foo(msg.msg, msg.val);
        }

        void foo(const std::string& msg, int val)
        {
            qDebug() <<  __FUNCTION__ << " -- Message:" << QString::fromStdString(msg) << ", val=" << val;
        }

    };

    Extc extc{loop};

    {
        Message msg {"karim", 29};


        std::string str{"karim"};
        int val {98};


        extc.onReceive(str, val);

       // std::this_thread::sleep_for(std::chrono::seconds(3));
    }
*/




}

void test_all()
{
    test_lambda_capture_move();
    test_mock_incoming_mqtt_event();


    test_lambda_shared_ptr();
    test_eventloop_eventhandler();
    test_eventloop_observer_signalslot();
    test_send_synchronous();
    test_send_synchronous_with_signalslots();
    test_mock_incoming_mqtt_event();
    test_mock_msupasyncremoterequest();


}

}
