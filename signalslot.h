#ifndef SIGNALSLOT_H
#define SIGNALSLOT_H

#include <QDebug>

#include <iostream>
#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

namespace test
{


/*!
 *
 *  NOTE: Un Signal peut etre utilise dans plusieurs Connection !
 *
 */

struct ConnectionBase;

struct SignalBase
{
    struct Slot
    {
        void *obj {nullptr};
        void *func {nullptr};
    };

    SignalBase() = default;
    ~SignalBase();
    SignalBase(SignalBase&& other) noexcept;
    SignalBase& operator= (SignalBase&& other) noexcept;

    SignalBase(const SignalBase&) = delete;
    SignalBase& operator= (const SignalBase&) = delete;


    std::vector<Slot> m_slots;
    std::vector<ConnectionBase *> m_connections;
    bool m_dirty {false};



};

struct ConnectionBase
{
    SignalBase * m_signal {nullptr};
    int m_index{0};

    ConnectionBase(SignalBase *sig, int idx)
        : m_signal(sig)
        , m_index(idx)
    {}

    ~ConnectionBase()
    {
        // Destruction de la connection:
        // Il faut signaler au signal que cette connection n'existe plus
        // TODO: On pourrait passer des shared_ptr/weak_ptr...a voir ?
        if (m_signal) {
            m_signal->m_slots[m_index].obj = nullptr;
            m_signal->m_slots[m_index].func = nullptr;
            m_signal->m_connections[m_index] = nullptr;
            m_signal->m_dirty = true;
        }
    }

};

/*!
 * \brief Connection est juste une enveloppe de ConnectionBase
 * On peut rajouter d'autres methodes
 */
struct Connection
{
    ConnectionBase * data {nullptr};
};

SignalBase::SignalBase(SignalBase&& other) noexcept
    : m_slots(std::move(other.m_slots))
    , m_connections(std::move(other.m_connections))
    , m_dirty(other.m_dirty)
{
    for (auto * connection : m_connections) {
        if (connection) {
            connection->m_signal = this;
        }
    }
}

SignalBase& SignalBase::operator= (SignalBase&& other) noexcept
{
    m_slots = std::move(other.m_slots);
    m_connections = std::move(other.m_connections);
    m_dirty = other.m_dirty;

    for (auto * connection : m_connections) {
        if (connection) {
            connection->m_signal = this;
        }
    }

    return *this;
}

SignalBase::~SignalBase()
{
    for (auto * connection : m_connections) {
        if (connection) {
            delete connection;
        }
    }
}

/*!
 * \brief Signal accepte des parametres variadics definis par ...Args
 */
template <typename ... Args>
struct Signal: public SignalBase
{
    /*!
     * \brief notify execute les slots
     * On peut passer d'autres arguments definis par CallArgs, le perfect forwarding devrait
     * permettre de caster les arguments correctement
     */
    template <typename ...CallArgs>
    void notify(CallArgs&& ...args)
    {

        for (auto i=0 ; i<m_slots.size() ; i++)
        {
            auto& slot = m_slots[i];
            reinterpret_cast<void(*)(void*, Args...)>(slot.func)(&slot.obj, std::forward<CallArgs>(args)...);
        }

    }

    template<auto FuncPtr, class Class>
    Connection connect(Class* object)
    {
        // Prochain index dans les connections
        size_t idx = m_connections.size();
        auto& call = m_slots.emplace_back();

        // Pour les pointeurs de fonction membre, on enveloppe son appel dans une lambda
        // le +[] pour convertir la lambda en (void *). c.f. c++ specs <built.over>
        call.obj = object;
        call.func = reinterpret_cast<void*>(+[](void* obj, Args ... args) {((*reinterpret_cast<Class**>(obj))->*FuncPtr)(args...); });
        ConnectionBase* conn = new ConnectionBase(this, idx);
        m_connections.push_back(conn);
        return { conn };
    }
};

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

    mySignal.connect<&AnyReceiver::anotherSlot>(&rcv);
    mySignal.connect<&AnyReceiver::differentSlot>(&rcv);
    mySignal.connect<&PsuInterface::slot1>(&psu);


    mySignal.notify(5, 28.8);

}

}



#endif // SIGNALSLOT_H
