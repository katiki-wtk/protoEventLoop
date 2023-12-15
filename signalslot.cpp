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

namespace tests
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


struct NonTrivialFunctor {

    ~NonTrivialFunctor() {
        qDebug() << __FUNCTION__ << "DTOR !!";

    }

    void operator()(int a, double d) {
        qDebug() << __FUNCTION__ << "NonTrivialFunctor: " << a << d;
    }
};

void simple_function(int a, double b)
{
    qDebug() << __FUNCTION__ << ": a=" << a << ", b=" << b;
}




void test_pmf()
{
    Signal<int, double> mySignal;

    AnyReceiver rcv;

    mySignal.connect<&AnyReceiver::anotherSlot>(&rcv);
    Connection conn = mySignal.connect<&AnyReceiver::differentSlot>(&rcv);
    mySignal.notify(5, 28.8);
    conn.disconnect();
}


void test_nontrivial_functor()
{
    Signal<int, double> mySignal;
    mySignal.connect(NonTrivialFunctor{});
    mySignal.notify(28, 19.77);

}

void test_signals()
{




    Signal<int, double> mySignal;

    AnyReceiver rcv;

    PsuInterface psu;

    qDebug() << "******* Connect Test";
    mySignal.connect<&AnyReceiver::anotherSlot>(&rcv);
    Connection conn = mySignal.connect<&AnyReceiver::differentSlot>(&rcv);
    mySignal.connect<&PsuInterface::slot1>(&psu);
    mySignal.connect(simple_function);

    mySignal.notify(5, 28.8);

    qDebug() << "******* Disconnect Test";
    conn.disconnect();
    mySignal.notify(9,99);


    qDebug() << "******* Lambda test";

    /*
    auto ff = [&mySignal](int i, double d) {
        qDebug() << "FunctionPointer: i=" << i << ", d=" << d;
        qDebug() << "Beware of recursivity....";
        mySignal.notify(29,20);
    };
*/

    mySignal.connect([&mySignal](int i, double d) {
        qDebug() << "FunctionPointer: i=" << i << ", d=" << d;
        qDebug() << "Beware of recursivity....";
     //   mySignal.notify(29,20);
    });

    mySignal.connect(NonTrivialFunctor{});

    mySignal.notify(28, 19.77);

    qDebug() << "******* Functor Test";
    struct Functor
    {
        void operator()(int i, double d)
        {
            qDebug() << "Functor: i=" << i << ", d=" << d;
        }
    };

    Functor functor;
    mySignal.connect(functor);
    mySignal.notify(19, 75);

}
}

}
