#include "signalslot.h"


namespace test
{

Connection::Connection(BasicConnection conn)
    : data(conn.data())
{
    data->m_used = true;
}

Connection::~Connection() {
    disconnect();
}

Connection::Connection(Connection&& other) noexcept
    : data(other.data) {
    other.data = nullptr;
}

Connection& Connection::operator=(Connection&& other) noexcept
{
    disconnect();
    data = other.data;
    other.data = nullptr;
    return *this;
}



void Connection::disconnect()
{
    delete data;
    data = nullptr;
}


SignalBase::SignalBase(SignalBase&& other) noexcept
    : m_connections(std::move(other.m_connections))
    , m_dirty(other.m_dirty)
{
    for (auto& connection : m_connections) {
        if (connection.conn) {
            connection.conn->m_signal = this;
        }
    }
}

SignalBase& SignalBase::operator= (SignalBase&& other) noexcept
{
    m_connections = std::move(other.m_connections);
    m_dirty = other.m_dirty;

    for (auto& connection : m_connections) {
        if (connection.conn) {
            connection.conn->m_signal = this;
        }
    }

    return *this;
}

SignalBase::~SignalBase()
{
    for (auto& connection : m_connections) {
        auto * conn = connection.conn;
        if (conn) {
            if (conn->m_used) {
                conn->m_signal = nullptr;
            }
            else {
                delete conn;
            }
        }
    }
}


void test_signals()
{
    struct AnyReceiver
    {
        void anotherSlot(int a, double b) {
            qDebug() << __FUNCTION__ << a << b;
        }


        void differentSlot(int a, int b) {
            qDebug() << __FUNCTION__ << a << b;
        }
    };


    struct PsuInterface {
        void slot1(int a, double b) {
            qDebug() << __FUNCTION__ << a << b;
        }
    };


    Signal<int, double> mySignal;

    AnyReceiver rcv;

    PsuInterface psu;

    std::cout << "******* Connect Test" << std::endl;
    mySignal.connect<&AnyReceiver::anotherSlot>(&rcv);
    Connection conn = mySignal.connect<&AnyReceiver::differentSlot>(&rcv);
    mySignal.connect<&PsuInterface::slot1>(&psu);

    mySignal.notify(5, 28.8);


    std::cout << "******* Disconnect Test" << std::endl;
    conn.disconnect();
    mySignal.notify(9,99);


    std::cout << "******* Lambda test" << std::endl;


    mySignal.connect([](int i, double d) {
            std::cout << "FunctionPointer: i=" << i << ", d=" << d << std::endl;
    });

    mySignal.notify(28, 19.77);

    std::cout << "******* Functor Test" << std::endl;
    struct Functor
    {
        void operator()(int i, double d)
        {
            std::cout << "Functor: i=" << i << ", d=" << d << std::endl;
        }
    };

    Functor functor;
    mySignal.connect(functor);
    mySignal.notify(19, 75);

}

}
